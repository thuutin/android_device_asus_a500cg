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

#ifndef ANDROID_MULTIDISPLAYTYPE_H
#define ANDROID_MULTIDISPLAYTYPE_H


//namespace android {
//namespace intel {

#define MDS_NO_ERROR (0)
#define MDS_ERROR    (-1)

#define MDS_VIDEO_SESSION_MAX_VALUE       (16)

enum {
    MDS_GET_MODE = 1,//IBinder::FIRST_CALL_TRANSACTION,
    MDS_SET_MODEPOLICY,
    MDS_NOTIFY_WIDI,
    MDS_NOTIFY_MIPI,
    MDS_NOTIFY_HOTPLUG,
    MDS_HDMI_POWER_OFF,
    MDS_PREPARE_FOR_VIDEO,
    MDS_GET_VIDEO_STATE,
    MDS_UPDATE_VIDEOINFO,
    MDS_GET_HDMIMODE_INFO_COUNT,
    MDS_GET_HDMIMODE_INFO,
    MDS_SET_HDMIMODE_INFO,
    MDS_REGISTER_LISTENER,
    MDS_UNREGISTER_LISTENER,
    MDS_SET_HDMISCALE_TYPE,
    MDS_SET_HDMISCALE_STEP,
    MDS_GET_HDMIDEVICE_CHANGE,
    MDS_GET_VIDEO_INFO,
    MDS_GET_DISPLAY_CAPABILITY,
};

typedef struct _MDSVideoInfo {
    bool isPlaying;
    bool isProtected;
    int  frameRate;
    int  displayW;
    int  displayH;
    bool isInterlaced;
} MDSVideoSourceInfo;

typedef struct _MDSHDMITiming {
    unsigned int    refresh;
    int             width;
    int             height;
    int             interlace;
    int             ratio;
} MDSHDMITiming;

typedef enum _MDSMessageType {
    MDS_MODE_CHANGE               = 0x1,
    MDS_SET_TIMING                = 0x1 << 1,
    MDS_SET_VIDEO_STATUS          = 0x1 << 2,
} MDSMessageType;

typedef enum _MDSMode {
    MDS_MODE_NONE      = 0,
    MDS_MIPI_ON        = 0x1,

    MDS_DVI_CONNECTED  = 0x1 << 1,
    MDS_HDMI_CONNECTED = 0x1 << 3,
    MDS_HDMI_ON        = 0x1 << 4,
    MDS_HDMI_CLONE     = 0x1 << 5,
    MDS_HDMI_VIDEO_EXT = 0x1 << 6,
    MDS_WIDI_ON        = 0x1 << 7,

    MDS_VIDEO_PLAYING  = 0x1 << 23,
    MDS_HDCP_ON        = 0x1 << 24,
    MDS_OVERLAY_OFF    = 0x1 << 25,
} MDSMode;

enum {
    MDS_HDMI_ON_NOT_ALLOWED  = 0,
    MDS_HDMI_ON_ALLOWED      = 1,
    MDS_MIPI_OFF_NOT_ALLOWED = 2,
    MDS_MIPI_OFF_ALLOWED     = 3,
};

enum {
    MDS_HW_SUPPORT_HDMI = 0x1,
    MDS_HW_SUPPORT_WIDI = (0x1 << 1),
};

typedef enum {
    MDS_VIDEO_PREPARING   = 1,
    MDS_VIDEO_PREPARED    = 2,
    MDS_VIDEO_UNPREPARING = 3,
    MDS_VIDEO_UNPREPARED  = 4,
} MDS_VIDEO_STATE;

//}; // namespace intel
//}; // namespace android

#endif

