/*
 * Copyright (c) 2012-2013, Intel Corporation. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

package com.intel.multidisplay;

import android.app.ActivityManager;
import android.app.ActivityManager.RunningTaskInfo;

import android.content.BroadcastReceiver;
import android.content.IntentFilter;
import android.os.Bundle;
import android.app.ActivityManagerNative;
import android.content.Context;
import android.content.Intent;
import android.os.Handler;
import android.os.Message;
import android.os.PowerManager;
import android.os.PowerManager.WakeLock;
import android.os.UEventObserver;
import android.util.Slog;
import android.telephony.TelephonyManager;
import android.provider.Settings;
import android.content.ContentResolver;
import android.database.ContentObserver;

import android.hardware.display.DisplayManager;
import android.hardware.display.WifiDisplayStatus;
import java.util.List;
import com.intel.multidisplay.DisplaySetting;

import android.view.MotionEvent;
import android.view.WindowManagerPolicy;
import android.view.WindowManagerPolicy.PointerEventListener;

/**
 * <p>DisplayObserver.
 */
public class DisplayObserver {
    private static final String TAG = "MultiDisplay";
    private static final boolean LOG = true;

    // Assuming unplugged (i.e. 0) for initial state, assign initial state in init() below.
    //private final int ROUTE_TO_SPEAKER = 0;
    //private final int ROUTE_TO_HDMI    = 1;
    //private int mAudioRoute =  ROUTE_TO_SPEAKER;
    //private int mPreAudioRoute = -1;
    private String mHDMIName;
    private int mBootStatus = 1;
    private int mHoriRatio = 5;
    private int mVertRatio = 5;

    // indicate HDMI connect state
    private boolean mHDMIConnected = false;
    private boolean mDVIConnected = false;
    // 0: not external display, DisplaySetting.EDP_HDMI, DisplaySetting.EDP_DVI
    private int mEDPlugIn = 0;

    private Context mContext;
    private WakeLock mWakeLock;  // held while there is a pending route change
    private boolean mHasIncomingCall = false;
    private boolean mInCallScreenFinished = true;
    private boolean mAllowModeSet = true;
    private DisplaySetting mDs;

    // WIDI
    private boolean mWidiConnected = false;
    private int mVppStatus = 0;

    //Message need to handle
    //private final int MSG_AUDIO_ROUTER_CHANGE = 0;
    private final int MSG_EDP_CONNECTION_CHANGE = 1;
    private final int MSG_ENABLE_EDP_SETTING = 2;
    private final int MSG_INPUT_TIMEOUT = 3;
    private final int MSG_START_MONITORING_INPUT = 4;
    private final int MSG_STOP_MONITORING_INPUT = 5;
    private final int INPUT_TIMEOUT_MSEC = 5000;

    // Broadcast receiver for device connections intent broadcasts
    private final BroadcastReceiver mReceiver = new DisplayObserverBroadcastReceiver();

    private final class DisplayObserverTouchEventListener implements PointerEventListener {
        private DisplayObserver mObserver;

        public DisplayObserverTouchEventListener(DisplayObserver observer) {
            mObserver = observer;
        }

        @Override
        public void onPointerEvent(MotionEvent motionEvent) {
            if (motionEvent.getAction() == motionEvent.ACTION_CANCEL) {
                return;
            }
            mObserver.onInputEvent();
        }
    }

    private DisplayObserverTouchEventListener mTouchEventListener = null;
    private WindowManagerPolicy.WindowManagerFuncs mWindowManagerFuncs;

    public void onInputEvent() {
        logv("input is active");
        mDs.updateInputState(true);
        if (mHandler.hasMessages(MSG_INPUT_TIMEOUT))
            mHandler.removeMessages(MSG_INPUT_TIMEOUT);

        mHandler.sendEmptyMessageDelayed(MSG_INPUT_TIMEOUT, INPUT_TIMEOUT_MSEC);
    }

    private void startMonitoringInput() {
        if (mTouchEventListener != null)
            return;
        if (mWindowManagerFuncs == null) {
            logv("invalid WindowManagerFuncs");
            return;
        }

        logv("start monitoring input");
        mTouchEventListener =
            new DisplayObserverTouchEventListener(this);
        if (mTouchEventListener == null) {
           logv("invalid input event receiver.");
           return;
        }
        mWindowManagerFuncs.registerPointerEventListener(
                mTouchEventListener);

        // start "input idle" count down
        mHandler.sendEmptyMessageDelayed(MSG_INPUT_TIMEOUT, INPUT_TIMEOUT_MSEC);
    }

    private void stopMonitoringInput() {
        if (mWindowManagerFuncs == null ||
                mTouchEventListener == null)
            return;

        logv("stop monitoring input");
        mWindowManagerFuncs.unregisterPointerEventListener(
                mTouchEventListener);
        mTouchEventListener = null;

        if (mHandler.hasMessages(MSG_INPUT_TIMEOUT)) {
            mHandler.removeMessages(MSG_INPUT_TIMEOUT);
        }
    }

    public DisplayObserver(Context context,
            WindowManagerPolicy.WindowManagerFuncs funcs) {
        if (funcs == null) {
            logv("invalid WindowManagerFuns, MDS couldn't handle input envent");
        }

        mContext = context;
        mWindowManagerFuncs = funcs;
        mDs = new DisplaySetting();
        IntentFilter intentFilter = new IntentFilter(TelephonyManager.ACTION_PHONE_STATE_CHANGED);
        intentFilter.addAction(mDs.MDS_GET_HDMI_INFO);
        intentFilter.addAction(mDs.MDS_SET_HDMI_MODE);
        intentFilter.addAction(mDs.MDS_SET_HDMI_SCALE);
        intentFilter.addAction(mDs.MDS_SET_HDMI_STEP_SCALE);
        intentFilter.addAction(mDs.MDS_SET_VPP_STATUS);
        intentFilter.addAction(DisplayManager.ACTION_WIFI_DISPLAY_STATUS_CHANGED);

        mContext.registerReceiver(mReceiver, intentFilter);
        PowerManager pm = (PowerManager)context.getSystemService(Context.POWER_SERVICE);
        mWakeLock = pm.newWakeLock(PowerManager.PARTIAL_WAKE_LOCK, "DisplayObserver");
        mWakeLock.setReferenceCounted(false);
        mDs.setMdsMessageListener(mListener);
        mHDMIConnected = mDVIConnected = false;
        int mode = mDs.getMode();
        if ((mode & mDs.HDMI_CONNECTED_BIT) == mDs.HDMI_CONNECTED_BIT) {
            mHDMIConnected = true;
            mEDPlugIn = mDs.EDP_HDMI;
            //switchAudio("HOTPLUG", ROUTE_TO_HDMI);
        } else {
            if((mode & mDs.DVI_CONNECTED_BIT) == mDs.DVI_CONNECTED_BIT) {
                mDVIConnected = true;
                mEDPlugIn = mDs.EDP_DVI;
            }
            //switchAudio("HOTPLUG", ROUTE_TO_SPEAKER);
        }
        logv("MDS Mode: " + mode + ", HDMI: " + mHDMIConnected + ", DVI: " + mDVIConnected);
    }

    protected void finalize() throws Throwable {
        stopMonitoringInput();
        mContext.unregisterReceiver(mReceiver);
        super.finalize();
    }

    private void logv(String s) {
        if (LOG) {
            Slog.v(TAG, s);
        }
    }

    DisplaySetting.onMdsMessageListener mListener =
                        new DisplaySetting.onMdsMessageListener() {
        public boolean onMdsMessage(int msg, int value) {
            if (msg == mDs.MDS_MSG_MODE_CHANGE) {
                boolean isHdmiConnected = ((value & mDs.HDMI_CONNECTED_BIT) != 0);
                boolean isDviConnected = ((value & mDs.DVI_CONNECTED_BIT) != 0);
                mAllowModeSet = !((value & mDs.VIDEO_ON_BIT) != 0);

                // start monitor input only when HDMI/Wireless Display
                // is connected AND the video is being played.
                if ((isHdmiConnected || isDviConnected || (value & mDs.WIDI_CONNECTED_BIT) != 0) &&
                        ((value & mDs.VIDEO_ON_BIT) != 0)) {
                    mHandler.sendEmptyMessage(MSG_START_MONITORING_INPUT);
                } else {
                    mHandler.sendEmptyMessage(MSG_STOP_MONITORING_INPUT);
                }

                mHandler.sendMessage(mHandler.obtainMessage(
                                MSG_ENABLE_EDP_SETTING, (mAllowModeSet ? 1 : 0), -1));

                logv("Current state: " + isHdmiConnected + ", " + isDviConnected + ", " + mAllowModeSet);
                boolean connectionChanged = true;
                // Check HDMI connection status
                logv("Previous state: " + mHDMIConnected + ", " + mDVIConnected);
                if (!mHDMIConnected && isHdmiConnected) {
                    mHDMIConnected = true;
                    mDVIConnected  = false; // clear DVI status
                    mEDPlugIn = mDs.EDP_HDMI; // HDMI plug in event
                } else if (mHDMIConnected && !isHdmiConnected) {
                    mHDMIConnected = false;
                } else
                    connectionChanged = false;
                if (connectionChanged) {
                    logv("HDMI connection status is change, " + mHDMIConnected);
                    mHandler.sendMessage(mHandler.obtainMessage(
                                MSG_EDP_CONNECTION_CHANGE,
                                mDs.EDP_HDMI, (mHDMIConnected ? 1 : 0)));
                    //switchAudio("HOTPLUG", (mHDMIConnected ? ROUTE_TO_HDMI : ROUTE_TO_SPEAKER));
                    return true;
                }
                // Check DVI status if HDMI isn't connected
                if (!connectionChanged) {
                    connectionChanged = true;
                    if (!mDVIConnected && isDviConnected) {
                        mDVIConnected = true;
                        mHDMIConnected = false; // clear HDMI state
                        mEDPlugIn = mDs.EDP_DVI; // DVI plug in event
                    } else if (mDVIConnected && !isDviConnected) {
                        mDVIConnected = false;
                    } else
                        connectionChanged = false;
                    if (connectionChanged) {
                        logv("DVI connection status is change, " + mDVIConnected);
                        mHandler.sendMessage(mHandler.obtainMessage(
                                    MSG_EDP_CONNECTION_CHANGE,
                                    mDs.EDP_DVI, (mDVIConnected ? 1 : 0)));
                        //switchAudio("HOTPLUG", ROUTE_TO_SPEAKER); // still route to speaker
                    }
                }
            }
            return true;
        };
    };
/*
    private synchronized void switchAudio(String newName, int newState) {
        // Retain only relevant bits
        int delay = 0;
        if (newState == mAudioRoute) {
            //logv("Same Audio output, Don't set");
            return;
        }
        logv("Audio output switches from " +
                (mAudioRoute == ROUTE_TO_SPEAKER ? "SPEAKER":"HDMI") +
                " to " + (newState == ROUTE_TO_SPEAKER ? "SPEAKER":"HDMI"));

        mHDMIName = newName;
        mPreAudioRoute = mAudioRoute;
        mAudioRoute = newState;

        if (newState == ROUTE_TO_SPEAKER) {
            Intent intent = new Intent(AudioManager.ACTION_AUDIO_BECOMING_NOISY);
            mContext.sendBroadcast(intent);
            // Add delay of 500ms when audio route to speaker to
            // avoid audio tearing event to occur,
            // which fails when initialize the video decoder
            delay = 500;
        }
        mWakeLock.acquire();
        if (newState == ROUTE_TO_HDMI && mPreAudioRoute == ROUTE_TO_SPEAKER) {
            // remove the last msg which switching audio to speaker if it exists
            // because this msg has 500ms delay
            mHandler.removeMessages(MSG_AUDIO_ROUTER_CHANGE);
        }
        mHandler.sendMessageDelayed(
                            mHandler.obtainMessage(MSG_AUDIO_ROUTER_CHANGE,
                                    mAudioRoute, mPreAudioRoute, mHDMIName),
                            delay);
    }

    private synchronized final void sendAudioIntents(int cur, int prev, String name) {
        if (cur == ROUTE_TO_HDMI || (prev == ROUTE_TO_HDMI))
            sendAudioIntent(cur, name);
    }

    private final void sendAudioIntent(int state, String name) {
        //logv("state:" + state + " name:" + name);
        //  Pack up the values and broadcast them to everyone
        Intent intent = new Intent(Intent.ACTION_HDMI_AUDIO_PLUG);
        intent.addFlags(Intent.FLAG_RECEIVER_REGISTERED_ONLY);
        intent.putExtra("state", state);
        intent.putExtra("name", name);

        // Should we require a permission?
        ActivityManagerNative.broadcastStickyIntent(intent, name, 0);
    }
*/
    // Don't allow to set HDMI setting, original patch is  167949
    private final void enableHdmiSetting(int enable) {
        logv("Enable Disable External DP Mode Setting " + enable);
        Intent outIntent = new Intent(mDs.MDS_ALLOW_MODE_SET);
        Bundle mBundle = new Bundle();
        mBundle.putBoolean("allowModeSet", (enable == 0 ? false : true));
        outIntent.putExtras(mBundle);
        mContext.sendBroadcast(outIntent);
    }


    // Broadcast external display connection state if exernal display device is plug in/out
    // type:  Externale display type, refer DisplaySetting.java
    // state: connection state
    private final void sendEdpConnectionChangeIntent(int type, int state) {
        logv("Dislay " + type + ", state " + state);
        Intent intent = new Intent(mDs.MDS_EDP_HOTPLUG);
        intent.addFlags(Intent.FLAG_RECEIVER_REGISTERED_ONLY);
        intent.putExtra("type", type);
        intent.putExtra("state", state);

        // Should we require a permission?
        ActivityManagerNative.broadcastStickyIntent(intent, "MDS_EDP_PLUG", 0);
    }

    private final Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            //logv("Handle message " + (String)msg.what);
            switch(msg.what) {
            //case MSG_AUDIO_ROUTER_CHANGE:
            //    sendAudioIntents(msg.arg1, msg.arg2, (String)msg.obj);
            //    mWakeLock.release();
            //    break;
            case MSG_EDP_CONNECTION_CHANGE:
                sendEdpConnectionChangeIntent(msg.arg1, msg.arg2);
                break;
            case MSG_ENABLE_EDP_SETTING:
                enableHdmiSetting(msg.arg1);
                break;
            case MSG_INPUT_TIMEOUT:
                logv("input is idle");
                mDs.updateInputState(false);
                break;
            case MSG_START_MONITORING_INPUT:
                startMonitoringInput();
                break;
            case MSG_STOP_MONITORING_INPUT:
                stopMonitoringInput();
                break;
            }
        }
    };

    private class DisplayObserverBroadcastReceiver extends BroadcastReceiver {
        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            if (action.equals(TelephonyManager.ACTION_PHONE_STATE_CHANGED)) {
                if (TelephonyManager.EXTRA_STATE == null ||
                        TelephonyManager.EXTRA_STATE_RINGING == null)
                    return;
                String extras = intent.getStringExtra(TelephonyManager.EXTRA_STATE);
                if (extras == null)
                    return;
                if (extras.equals(TelephonyManager.EXTRA_STATE_RINGING)) {
                    mHasIncomingCall = true;
                    mInCallScreenFinished = false;
                    logv("Incoming call is initiated");
                    mDs.updatePhoneCallState(true);
                } else if (extras.equals(TelephonyManager.EXTRA_STATE_IDLE)) {
                    mHasIncomingCall = false;
                    mInCallScreenFinished = true;
                    logv("Call is terminated and Incallscreen disappeared");
                    mDs.updatePhoneCallState(false);
                }
            } else if (action.equals(mDs.MDS_GET_HDMI_INFO)) {
                logv("State: " + mHDMIConnected + ", " + mDVIConnected + ", " + mBootStatus + ", " + mEDPlugIn);
                // Handle HDMI_GET_INFO ACTION
                if (!mHDMIConnected && !mDVIConnected) {
                    return;
                }
                // Get Number of Timing Info
                int Count = mDs.getHdmiInfoCount();
                Intent outIntent = new Intent(mDs.MDS_HDMI_INFO);
                outIntent.addFlags(Intent.FLAG_RECEIVER_REGISTERED_ONLY);
                Bundle mBundle = new Bundle();

                int[] arrWidth = new int[Count];
                int[] arrHeight = new int[Count];
                int[] arrRefresh = new int[Count];
                int[] arrInterlace = new int[Count];
                int[] arrRatio = new int[Count];
                mDs.getHdmiTiming(arrWidth, arrHeight, arrRefresh, arrInterlace, arrRatio);
                mBundle.putSerializable("width", arrWidth);
                mBundle.putSerializable("height", arrHeight);
                mBundle.putSerializable("refresh", arrRefresh);
                mBundle.putSerializable("interlace", arrInterlace);
                mBundle.putSerializable("ratio", arrRatio);
                mBundle.putInt("count", Count);
                mBundle.putInt("bootStatus", mBootStatus);
                mBundle.putBoolean("hasIncomingCall",mHasIncomingCall);
                mBundle.putBoolean("allowModeSet",mAllowModeSet);
                mBundle.putInt("eDPlugin", mEDPlugIn);
                mEDPlugIn = 0; // clear
                mBundle.putInt("bootStatus", mBootStatus);
                mBootStatus = 0; // clear
                outIntent.putExtras(mBundle);
                mContext.sendBroadcast(outIntent);
            } else if (action.equals(mDs.MDS_SET_HDMI_MODE)) {
                // Set Specified Timing Info: width, height ,refresh, interlace
                Bundle extras = intent.getExtras();
                if (extras == null)
                    return;
                int Width = extras.getInt("width", 0);
                int Height = extras.getInt("height", 0);
                int Refresh = extras.getInt("refresh", 0);
                int Ratio = extras.getInt("ratio", 0);
                int Interlace = extras.getInt("interlace", 0);
                int vic = extras.getInt("vic", 0);
                logv("Set a new mode: " + Width + "x" + Height + "@"
                        + Refresh + "," + Interlace + "," + Ratio + "," + vic);
                /*
                 * Use native_setHdmiInfo for processing vic information too,
                 * instead of creating a new interface or modifying the existing
                 * one. When vic is present, set width to vic value and height
                 * '0' to differentiate.
                 */
                if (vic != 0) {
                    logv("set info vic = " + vic);
                    Width = vic;
                    Height = 0;
                }
                if (!mDs.setHdmiTiming(Width, Height, Refresh, Interlace, Ratio))
                    logv("Set HDMI Timing Info error");
            } else if (action.equals(mDs.MDS_SET_HDMI_SCALE)) {
                Bundle extras = intent.getExtras();
                if (extras == null)
                     return;
                int ScaleType = extras.getInt("Type", 0);

                logv("set scale info Type:" +  ScaleType);
                if (!mDs.setHdmiScaleType(ScaleType))
                    logv("Set HDMI Scale error");
            } else if (action.equals(mDs.MDS_SET_HDMI_STEP_SCALE)) {
                Bundle extras = intent.getExtras();
                if (extras == null)
                     return;
                int Step = extras.getInt("Step", 0);
                int Orientation = extras.getInt("Orientation", 0);
                logv("orientation" + Orientation);
                if (Orientation == 0)
                    mHoriRatio = Step;
                else
                    mVertRatio = Step;
                logv("set scale info step:" +  Step);
                if(!mDs.setHdmiOverscan(mHoriRatio, mVertRatio))
                    logv("Set HDMI Step Scale error");
            } else if(action.equals(DisplayManager.ACTION_WIFI_DISPLAY_STATUS_CHANGED)) {
                boolean connected = false;
                WifiDisplayStatus status = (WifiDisplayStatus)intent.getParcelableExtra(DisplayManager.EXTRA_WIFI_DISPLAY_STATUS);
                if (status == null)
                    return;
                if (status.getActiveDisplayState() ==
                        WifiDisplayStatus.DISPLAY_STATE_CONNECTED) {
                    connected = true;
                }
                if (mWidiConnected == connected)
                    return;
                logv("Update vpp status " + connected);
                mDs.setVppState(mDs.DISPLAY_VIRTUAL, connected, -1);
                mWidiConnected = connected;
            } else if(action.equals(mDs.MDS_SET_VPP_STATUS)) {
                logv("recive intent from vpp settings");
                Bundle extras = intent.getExtras();
                if (extras == null)
                     return;
                int status = extras.getInt("status", 0);

                logv("set vpp status:" +  status);
                mDs.setVppState(mDs.DISPLAY_VIRTUAL, mWidiConnected, status);
                mVppStatus = status;
            }
        }
    }
}
