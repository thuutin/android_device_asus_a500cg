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
//import android.media.AudioManager;
import android.telephony.TelephonyManager;
import android.provider.Settings;
import android.content.ContentResolver;
import android.database.ContentObserver;

import java.io.FileReader;
import java.io.FileNotFoundException;
import java.util.List;
import com.intel.multidisplay.DisplaySetting;

import android.view.WindowManagerPolicy;
import android.view.WindowManagerGlobal;
import android.view.IWindowManager;
import android.os.RemoteException;

import com.android.internal.view.RotationPolicy;
import android.content.res.Configuration;
import android.view.Surface;


/**
 * <p>DisplayObserver.
 */
public class DisplayObserver extends UEventObserver {
    private static final String TAG = "MultiDisplay";
    private static final boolean LOG = true;

    private final String HDMI_UEVENT_MATCH = "DEVPATH=/devices/pci0000:00/0000:00:02.0/drm/card0";

    // Assuming unplugged (i.e. 0) for initial state, assign initial state in init() below.
    //private final int ROUTE_TO_SPEAKER = 0;
    //private final int ROUTE_TO_HDMI    = 1;
    private static final int HDMI_CONNECT    = 1;
    private static final boolean ALLOW_MODE_SET = true;
    private final int FORCING_LANDSCAPE  = 1;
    private final int HDMI_HOTPLUG     = 2;
    private final int HDMI_POWER_OFF   = 3;
    private final int CHECK_INCALLSCREEN_ACTIVE = 4;
    //private int mAudioRoute =  ROUTE_TO_SPEAKER;
    //private int mPreAudioRoute = -1;
    private String mHDMIName;
    private int mEdidChange = 0;
    private int mDisplayBoot = 1;
    private int mHoriRatio = 5;
    private int mVertRatio = 5;

    // indicate HDMI connect state
    private int mHDMIConnected = 0;
    private int mHDMIPlugEvent = 0;

    private Context mContext;
    private WakeLock mWakeLock;  // held while there is a pending route change
    private boolean mHasIncomingCall = false;
    private boolean mInCallScreenFinished = true;
    private DisplaySetting mDs;
    private int mHdmiPolicy = DisplaySetting.HDMI_ON_ALLOWED;
    private int mMdsMode = 0;
    private int mDisplayCapability = 0;
    private boolean mIsExtMode = false;
    private boolean mSupportRotation = false;
    private boolean mForceLandscape = false;

    //Message need to handle
    // Broadcast receiver for device connections intent broadcasts
    private final BroadcastReceiver mReceiver = new DisplayObserverBroadcastReceiver();

    public DisplayObserver(Context context, WindowManagerPolicy.WindowManagerFuncs funcs) {
        mContext = context;
        mDs = new DisplaySetting();
        IntentFilter intentFilter = new IntentFilter(TelephonyManager.ACTION_PHONE_STATE_CHANGED);
        intentFilter.addAction(mDs.MDS_GET_HDMI_INFO);
        intentFilter.addAction(mDs.MDS_SET_HDMI_MODE);
        intentFilter.addAction(mDs.MDS_SET_HDMI_SCALE);
        intentFilter.addAction(mDs.MDS_SET_HDMI_STEP_SCALE);

        mContext.registerReceiver(mReceiver, intentFilter);
        PowerManager pm = (PowerManager)context.getSystemService(Context.POWER_SERVICE);
        mWakeLock = pm.newWakeLock(PowerManager.PARTIAL_WAKE_LOCK, "DisplayObserver");
        mWakeLock.setReferenceCounted(false);
        mDs.setMdsMessageListener(mListener);
        startObserving(HDMI_UEVENT_MATCH);
        mDisplayCapability = mDs.getDisplayCapability();
        if (checkDisplayCapability(mDs.HW_SUPPORT_HDMI) &&
                ((mDs.getMode() & mDs.HDMI_MODE_BIT) == mDs.HDMI_MODE_BIT)) {
            mHDMIConnected = 1;
            //update("HOTPLUG", ROUTE_TO_HDMI);
        } else {
            mHDMIConnected = 0;
            //update("HOTPLUG", ROUTE_TO_SPEAKER);
        }
        mSupportRotation = RotationPolicy.isRotationSupported(mContext);
    }

    protected void finalize() throws Throwable {
        mContext.unregisterReceiver(mReceiver);
        super.finalize();
    }

    private void logv(String s) {
        if (LOG) {
            Slog.v(TAG, s);
        }
    }

    private boolean checkDisplayCapability(int value) {
        if ((mDisplayCapability & value) == value)
            return true;
        return false;
    }

    DisplaySetting.onMdsMessageListener mListener =
                        new DisplaySetting.onMdsMessageListener() {
        public boolean onMdsMessage(int msg, int value) {
            if (msg == mDs.MDS_MODE_CHANGE) {
                //logv("mode is changed to 0x" + Integer.toHexString(value));
                mMdsMode = value;
                int forcing_landscape = 0;
                Message lmsg;
                boolean isExtMode = (mMdsMode & mDs.HDMI_EXTEND_MODE) != 0;
                if (mContext != null && mSupportRotation) {
                    if (isExtMode && !mIsExtMode) {
                        //logv("Entering ext mode");
                        forcing_landscape = 1;
                        mIsExtMode = true;
                        mHandler.removeMessages(FORCING_LANDSCAPE);
                        lmsg = mHandler.obtainMessage(FORCING_LANDSCAPE, forcing_landscape, 0);
                    } else if (!isExtMode && mIsExtMode) {
                        //logv("Leaving ext mode");
                        forcing_landscape = 0;
                        mIsExtMode = false;
                        mHandler.removeMessages(FORCING_LANDSCAPE);
                        lmsg = mHandler.obtainMessage(FORCING_LANDSCAPE, forcing_landscape, 0);
                    } else
                        return false;
                    mHandler.sendMessage(lmsg);
                }
            }
            return true;
        };
    };

    @Override
    public synchronized void onUEvent(UEventObserver.UEvent event) {
        if (event.toString().contains("HOTPLUG")) {
            logv("HDMI UEVENT: " + event.toString());
            int delay = 0;
            if (event.toString().contains("HOTPLUG_IN")) {
                delay = 0;
                mHDMIPlugEvent = 1;
            } else if (event.toString().contains("HOTPLUG_OUT")) {
                delay = 200;
                mHDMIPlugEvent = 0;
            } else
                return;
            mHandler.removeMessages(HDMI_HOTPLUG);
            Message msg = mHandler.obtainMessage(HDMI_HOTPLUG, mHDMIPlugEvent, 0);
            mHandler.sendMessageDelayed(msg, delay);
        }
    }
/*
    private synchronized void update(String newName, int newState) {
        // Retain only relevant bits
        int delay = 0;
        if (newState == mAudioRoute) {
            logv("Same Audio output, Don't set");
            return;
        }
        logv("Set Audio output from " +
                (mAudioRoute == ROUTE_TO_SPEAKER ? "SPEAKER":"HDMI") +
                " to " + (newState == ROUTE_TO_SPEAKER ? "SPEAKER":"HDMI"));

        mHDMIName = newName;
        mPreAudioRoute = mAudioRoute;
        mAudioRoute = newState;

        if (newState == ROUTE_TO_SPEAKER) {
            Intent intent = new Intent(AudioManager.ACTION_AUDIO_BECOMING_NOISY);
            mContext.sendBroadcast(intent);
        }
        mWakeLock.acquire();
        mHandler.sendMessageDelayed(mHandler.obtainMessage(HDMI_STATE_CHANGE,
                                    mAudioRoute,
                                    mPreAudioRoute,
                                    mHDMIName),
                                    delay);
    }

    private synchronized final void sendIntents(int cur, int prev, String name) {
        if (cur == ROUTE_TO_HDMI || (prev == ROUTE_TO_HDMI))
            sendIntent(1, cur, prev, name);
    }

    private final void sendIntent(int hdmi, int State, int prev, String Name) {
        //logv("State:" + State + " prev:" + prev + " Name:" + Name);
        //  Pack up the values and broadcast them to everyone
        Intent intent = new Intent(Intent.ACTION_HDMI_AUDIO_PLUG);
        intent.addFlags(Intent.FLAG_RECEIVER_REGISTERED_ONLY);
        intent.putExtra("state", State);
        intent.putExtra("name", Name);

        // Should we require a permission?
        ActivityManagerNative.broadcastStickyIntent(intent, Name, 0);
    }
*/
    // Broadcast external display connection state if exernal
    // display device is plug in/out
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

    private final void preNotifyHotplug(int event) {
        // no matter plug in or out, remove previous power off msg
        mHandler.removeMessages(HDMI_POWER_OFF);
        // set HDMI connect status per plug event
        if (event == HDMI_CONNECT)
            mHDMIConnected = 1;
        else
            mHDMIConnected = 0;

        sendEdpConnectionChangeIntent(1, mHDMIConnected);
    }

    private final void postNotifyHotplug(int event) {
        /* plug out event */
        if (event == 0) {
            //update("HOTPLUG", ROUTE_TO_SPEAKER);

            /*
             * delay 1s to power off in case
             * there are continous plug in/out
             */
            Message mmsg = mHandler.obtainMessage(HDMI_POWER_OFF);
            mHandler.sendMessageDelayed(mmsg, 1000);
        }// else
            //update("HOTPLUG", ROUTE_TO_HDMI);
    }

    private final void setHdmiPolicy(int policy) {
        if (!checkDisplayCapability(mDs.HW_SUPPORT_HDMI))
            return;
        if (policy != mHdmiPolicy &&
                (policy == mDs.HDMI_ON_ALLOWED || policy == mDs.HDMI_ON_NOT_ALLOWED)) {
            mDs.setModePolicy(policy);
            mHdmiPolicy = policy;
        }
    }

    private final void setLandscape(int landscape) {
        if (landscape == 1 && RotationPolicy.isRotationLocked(mContext)) {
            //logv("Rotation is already locked");
            return;
        }
        try {
            IWindowManager wm = WindowManagerGlobal.getWindowManagerService();
            if (landscape == 1 && !mForceLandscape) {
                //logv("Forcing to landscape mode");
                wm.freezeRotation(Surface.ROTATION_90);
                mForceLandscape = true;
            } else if (landscape == 0 && mForceLandscape){
                //logv("Revert from landscape mode");
                wm.thawRotation();
                mForceLandscape = false;
            }
        } catch (RemoteException exc) {
            logv("Unable to set landscape mode");
        }
    }

    private final Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            //logv("handle message = " + (String)msg.obj);
            switch(msg.what) {
            //case HDMI_STATE_CHANGE:
            //    sendIntents(msg.arg1, msg.arg2, (String)msg.obj);
            //    mWakeLock.release();
            //    break;
            case FORCING_LANDSCAPE:
                setLandscape(msg.arg1);
                break;
            case HDMI_HOTPLUG:
                synchronized(this) {
                    /* filter before msg which does not match with latest event */
                    if (mHDMIPlugEvent != msg.arg1)
                        return;

                    preNotifyHotplug(msg.arg1);

                    boolean ret = mDs.notifyHotPlug();
                    if (!ret) {
                        logv("fail to deal with hdmi hotlpug");
                        return;
                    }

                    postNotifyHotplug(msg.arg1);
                }
                break;
            case HDMI_POWER_OFF:
                mDs.setHdmiPowerOff();
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
                mHandler.removeMessages(CHECK_INCALLSCREEN_ACTIVE);
                if (extras.equals(TelephonyManager.EXTRA_STATE_RINGING)) {
                    mHasIncomingCall = true;
                    mInCallScreenFinished = false;
                    logv("Incoming call is initiated");
                    mDs.setModePolicy(mDs.MIPI_OFF_NOT_ALLOWED);
                    setHdmiPolicy(mDs.HDMI_ON_NOT_ALLOWED);
                } else if (extras.equals(TelephonyManager.EXTRA_STATE_IDLE)) {
                    mHasIncomingCall = false;
                    mInCallScreenFinished = true;
                    logv("Call is terminated and Incallscreen disappeared");
                    mDs.setModePolicy(mDs.MIPI_OFF_NOT_ALLOWED);
                    setHdmiPolicy(mDs.HDMI_ON_ALLOWED);
                }
            } else if (action.equals(mDs.MDS_GET_HDMI_INFO)) {
                // Handle HDMI_GET_INFO ACTION
                logv("HDMI is plugged "+ (mHDMIConnected == 1 ? "in" : "out"));
                if (mHDMIConnected != 0) {
                    // Get Number of Timing Info
                    int Count = mDs.getHdmiInfoCount();
                    mEdidChange = mDs.getHdmiDeviceChange();
                    logv("HDMI timing number:" + Count);
                    Intent outIntent = new Intent(mDs.MDS_HDMI_INFO);
                    outIntent.addFlags(Intent.FLAG_RECEIVER_REGISTERED_ONLY);
                    Bundle mBundle = new Bundle();
                    if (-1 != Count) {
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
                        mBundle.putInt("EdidChange", mEdidChange);
                        mBundle.putBoolean("mHasIncomingCall", mHasIncomingCall);
                        mBundle.putBoolean("allowModeSet", ALLOW_MODE_SET);
                        mBundle.putInt("bootStatus", mDisplayBoot);
                        mEdidChange = 0;
                        outIntent.putExtras(mBundle);
                        mContext.sendBroadcast(outIntent);
                    } else {
                        logv("fail to get HDMI info");
                    }
                }
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
                logv("set info <width:" + Width + "height:" + Height + "refresh:"
                        + Refresh + "interlace:" + Interlace + "ratio;" + Ratio + "VIC" + vic);
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
            }
            else if (action.equals(mDs.MDS_SET_HDMI_SCALE)) {
                Bundle extras = intent.getExtras();
                if (extras == null)
                     return;
                int ScaleType = extras.getInt("Type", 0);

                logv("set scale info Type:" +  ScaleType);
                if (!mDs.setHdmiScaleType(ScaleType))
                    logv("Set HDMI Scale error");
            }
            else if (action.equals(mDs.MDS_SET_HDMI_STEP_SCALE)) {
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
                if(!mDs.setHdmiScaleStep(mHoriRatio,mVertRatio))
                    logv("Set HDMI Step Scale error");
            }
        }
    }
}

