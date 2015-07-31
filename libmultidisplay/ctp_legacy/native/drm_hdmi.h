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


#ifndef _DRM_HDMI_H
#define _DRM_HDMI_H
#include <display/MultiDisplayType.h>


#define DRM_MIPI_OFF            (0)
#define DRM_MIPI_ON             (1)

#define DRM_HDMI_OFF            (0)
#define DRM_HDMI_CLONE          (1)
#define DRM_HDMI_VIDEO_EXT      (2)

#define DRM_HDMI_DISCONNECTED   (0)
#define DRM_HDMI_CONNECTED      (1)
#define DRM_DVI_CONNECTED       (2)

#define HDMI_TIMING_MAX (128)

bool drm_init();
void drm_cleanup();
int  drm_get_dev_fd();
int  drm_get_ioctl_offset();

bool drm_hdmi_isDeviceChanged(bool reset);
bool drm_hdmi_isSupported();
bool drm_hdmi_setHdmiVideoOn();
bool drm_hdmi_setHdmiVideoOff();
bool drm_hdmi_setHdmiPowerOff();
bool drm_hdmi_onHdmiDisconnected(void);
bool drm_hdmi_notify_audio_hotplug(bool connected);
// return 0 - disconnected, 1 - HDMI connected, 2 - DVI connected
int  drm_hdmi_getConnectionStatus();
// return number of unique (non-duplicated) modes
int  drm_hdmi_getModeInfo(
    int *pWidth,
    int* pHeight,
    int *pRefresh,
    int *pInterlace,
    int *pRatio);

bool drm_hdmi_setModeInfo(int width, int height, int refresh, int interlace, int ratio);

// get the best matched timing
bool drm_hdmi_getTiming(int mode, MDSHDMITiming* info);

// turn MIPI on or off
bool drm_mipi_setMode(int mode);

#endif // _DRM_HDMI_H
