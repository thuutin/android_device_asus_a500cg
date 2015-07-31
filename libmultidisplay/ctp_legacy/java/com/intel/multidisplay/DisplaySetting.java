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
 * Author: tianyang.zhu@intel.com
 */

package com.intel.multidisplay;

import java.io.IOException;
import android.util.Slog;


public class DisplaySetting {
    private final String TAG = "MultiDisplay";
    private final boolean LOG = true;

    // mode setting policy
    public static final int HDMI_ON_NOT_ALLOWED  = 0;
    public static final int HDMI_ON_ALLOWED = 1;
    public static final int MIPI_OFF_NOT_ALLOWED = 2;
    public static final int MIPI_OFF_ALLOWED = 3;

    // display mode
    // Don't change the enum, pls align with MultiDisplayType.h
    // bit 0: MIPI on/off
    // bit 3: HDMI connect status
    // bit 4: HDMI on/off
    // bit 5: HDMI clone mode
    // bit 6: HDMI extend mode
    public static final int MIPI_MODE_BIT           = 0x1;
    public static final int HDMI_CONNECT_STATUS_BIT = 0x1 << 3;
    public static final int HDMI_MODE_BIT           = 0x1 << 4;
    public static final int HDMI_CLONE_MODE         = 0x1 << 5;
    public static final int HDMI_EXTEND_MODE        = 0x1 << 6;

    // MDS message type
    public static final int MDS_MODE_CHANGE        = 0x1;
    public static final int MDS_ORIENTATION_CHANGE = 0x1 << 1;
    public static final int MDS_SET_TIMING         = 0x1 << 2;

    // MDS display capability
    public static final int HW_SUPPORT_HDMI = 0x1;
    public static final int HW_SUPPORT_WIDI = 0x1 << 1;

    public static final String MDS_EDP_HOTPLUG         = "android.intel.mds.EXTERNAL_DP_HOTPLUG";
    public static final String MDS_GET_HDMI_INFO       = "android.intel.mds.GET.HDMI_INFO";
    public static final String MDS_SET_HDMI_MODE       = "android.intel.mds.SET.HDMI_MODE";
    public static final String MDS_HDMI_INFO           = "android.intel.mds.HDMI_INFO";
    public static final String MDS_SET_HDMI_SCALE      = "android.intel.mds.SET.HDMI_SCALE";
    public static final String MDS_SET_HDMI_STEP_SCALE = "android.intel.mds.SET.HDMI_STEP_SCALE";
    public static final String MDS_GET_BOOT_STATUS     = "android.intel.mds.GET.BOOT_STATUS";
    public static final String MDS_BOOT_STATUS         = "android.intel.mds.BOOT_STATUS";
    public static final String MDS_ALLOW_MODE_SET       = "android.intel.mds.ALLOW_MODE_SET";

    private static onMdsMessageListener mListener = null;

    private static native boolean native_InitMDSClient(DisplaySetting thiz);
    private static native boolean native_DeInitMDSClient();
    private static native boolean native_setModePolicy(int policy);
    private static native int     native_getMode();
    private static native boolean native_setHdmiPowerOff();
    private static native boolean native_notifyHotPlug();
    private static native int     native_getHdmiTiming(int width[],
                                                int height[], int refresh[],
                                                int interlace[], int ratio[]);
    private static native boolean native_setHdmiTiming(int width, int height,
                            int refresh, int interlace, int ratio);
    private static native int     native_getHdmiInfoCount();
    private static native boolean native_setHdmiScaleType(int Type);
    private static native boolean native_setHdmiScaleStep(int Step, int Orientation);
    private static native int     native_getHdmiDeviceChange();
    private static native int     native_getDisplayCapability();
    private static native int     native_setPlayInBackground(boolean value, int playerId);
    private static native int     native_setHdcpStatus(int value);

    public DisplaySetting() {
        if (LOG) Slog.i(TAG, "Create a new DisplaySetting");
        native_InitMDSClient(this);
    }

    @Override
    protected void finalize() { native_DeInitMDSClient(); }

    public boolean setModePolicy(int policy) {
       return native_setModePolicy(policy);
    }

    public int getMode() {
        return native_getMode();
    }

    public boolean notifyHotPlug() {
        return native_notifyHotPlug();
    }

    public boolean setHdmiPowerOff() {
        return native_setHdmiPowerOff();
    }

    public void onMdsMessage(int event, int value) {
        if (mListener != null) {
            mListener.onMdsMessage(event, value);
        }
    }

    public void setMdsMessageListener(onMdsMessageListener listener) {
        if (mListener == null) {
            mListener = listener;
        }
    }

    public int getHdmiTiming(int width[], int height[],
                             int refresh[], int interlace[], int ratio[]) {
        return native_getHdmiTiming(width, height,
                                    refresh, interlace, ratio);
    }

    public boolean setHdmiTiming(int width, int height,
                        int refresh, int interlace, int ratio) {
        return native_setHdmiTiming(width, height,
                            refresh, interlace, ratio);
    }

    public interface onMdsMessageListener {
        boolean onMdsMessage(int event, int value);
    }

    public int getHdmiInfoCount() {
        return native_getHdmiInfoCount();
    }

    public boolean setHdmiScaleType(int Type) {
        return native_setHdmiScaleType(Type);
    }

    public boolean setHdmiScaleStep(int Step, int Orientation) {
        return native_setHdmiScaleStep(Step, Orientation);
    }

    public int getHdmiDeviceChange() {
        return native_getHdmiDeviceChange();
    }

    public int getDisplayCapability() {
        return native_getDisplayCapability();
    }

    public int setPlayInBackground(boolean enablePlayInBackground, int playerId) {
        return native_setPlayInBackground(enablePlayInBackground, playerId);
    }
}

