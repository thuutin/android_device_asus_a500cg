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


//#define LOG_NDEBUG 0

#include <utils/Log.h>
#include <errno.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include "linux/psb_drm.h"
#include "drm_hdmi.h"
#include "xf86drm.h"
#include "xf86drmMode.h"


#define HDMI_FORCE_VIDEO_ON_OFF 1
#define EDID_PRODUCT_INFO_LEN   8
#define PREFERRED_VREFRESH      60  // 60Hz
#define DRM_DEVICE_NAME         "/dev/card0"


typedef struct _drmContext {
    int drmFD;
    int ioctlOffset;
    bool hdmiSupported;
    bool newDevice;
    bool connected;
    int preferredModeIndex;
    bool modeValid;  // indicate whether modeSelected is valid
    MDSHDMITiming modeSelected;
    char productInfo[EDID_PRODUCT_INFO_LEN];
    drmModeConnectorPtr hdmiConnector;
} drmContext;

static drmContext gDrmCxt;

static drmModeConnector* getConnector(int fd, uint32_t connector_type)
{
    ALOGV("Entering %s, %d", __func__, connector_type);
    drmModeRes *resources = drmModeGetResources(fd);
    drmModeConnector *connector = NULL;
    int i;

    if (resources == NULL || resources->connectors == NULL) {
        ALOGE("%s: drmModeGetResources failed.", __func__);
        return NULL;
    }
    for (i = 0; i < resources->count_connectors; i++) {
        connector = drmModeGetConnector(fd, resources->connectors[i]);
        if (connector == NULL)
            continue;
        if (connector->connector_type == connector_type)
            break;

        drmModeFreeConnector(connector);
        connector = NULL;
    }
    drmModeFreeResources(resources);
    if (connector == NULL) {
        ALOGE("%s: Failed to get conector", __func__);
    }
    ALOGV("Leaving %s, %d", __func__, connector_type);
    return connector;
}

static drmModeConnectorPtr getHdmiConnector()
{
    if (gDrmCxt.hdmiConnector == NULL)
        gDrmCxt.hdmiConnector = getConnector(gDrmCxt.drmFD, DRM_MODE_CONNECTOR_DVID);
    if (gDrmCxt.hdmiConnector == NULL || gDrmCxt.hdmiConnector->modes == NULL) {
        ALOGI("Failed to get HDMI state, please check HDMI cable is connected or not");
        return NULL;
    }
    return gDrmCxt.hdmiConnector;
}

static inline bool drm_is_preferred_flags(unsigned int flags)
{
    // prefer 16:9 over 4:3  and progressive over interlaced.
    if ((flags & DRM_MODE_FLAG_PAR16_9) && !(flags & DRM_MODE_FLAG_INTERLACE))
        return true;
    return false;
}
static void drm_select_preferredmode(drmModeConnectorPtr connector)
{
    int index_preferred = -1;
    int hdisplay = 0, vdisplay = 0;
    int index_720P = -1, index_1080P = -1, index_max = -1;
    int i;

    drmModeModeInfoPtr mode;
    for (i = 0; i < connector->count_modes; i++) {
        mode = connector->modes + i;
        ALOGV("mode #%d: %dx%d@%dHz, flags = %#x", i,
                connector->modes[i].hdisplay,
                connector->modes[i].vdisplay,
                connector->modes[i].vrefresh,
                connector->modes[i].flags);

        if (mode->type & DRM_MODE_TYPE_PREFERRED) {
            ALOGI("The preferred mode is %d", i);
            index_preferred = i;
            break;
        }
        if (mode->hdisplay == 1280 &&
            mode->vdisplay == 720 &&
            mode->vrefresh == PREFERRED_VREFRESH) {
            if (index_720P == -1) {
                index_720P = i;
            } else  if (drm_is_preferred_flags(mode->flags)) {
                index_720P = i;
            }
        }
        if (mode->hdisplay == 1920 &&
            mode->vdisplay == 1080 &&
            mode->vrefresh == PREFERRED_VREFRESH) {
            if (index_1080P == -1) {
                index_1080P = i;
            } else  if (drm_is_preferred_flags(mode->flags)) {
                index_1080P = i;
            }
        }
        if (mode->hdisplay > hdisplay &&
            mode->vdisplay > vdisplay &&
            mode->vrefresh == PREFERRED_VREFRESH) {
            hdisplay = mode->hdisplay;
            vdisplay = mode->vdisplay;
            index_max = i;
        } else if (mode->hdisplay == hdisplay &&
                   mode->vdisplay == vdisplay &&
                   mode->vrefresh == PREFERRED_VREFRESH) {
            if (drm_is_preferred_flags(mode->flags)) {
                index_max = i;
            }
        }
    }

    gDrmCxt.preferredModeIndex = 0;
    if (index_preferred != -1 &&
            connector->modes[index_preferred].vrefresh == PREFERRED_VREFRESH) {
        gDrmCxt.preferredModeIndex = index_preferred;
    } else if (index_1080P != -1) {
        gDrmCxt.preferredModeIndex = index_1080P;
    } else if (index_720P != -1) {
        gDrmCxt.preferredModeIndex = index_720P;
    } else if (index_max != -1) {
        gDrmCxt.preferredModeIndex = index_max;
    }

    index_preferred = gDrmCxt.preferredModeIndex;
    ALOGI("Preferred mode is: %dx%d@%dHz, index = %d",
            connector->modes[index_preferred].hdisplay,
            connector->modes[index_preferred].vdisplay,
            connector->modes[index_preferred].vrefresh,
            index_preferred);

}

static void drm_hdmi_setTiming(drmModeConnectorPtr connector, int index, MDSHDMITiming* info)
{
    if (index >= connector->count_modes) {
        ALOGE("%s: index is out of range.", __func__);
        memset(info, 0, sizeof(MDSHDMITiming));
        return;
    }

    drmModeModeInfoPtr mode = connector->modes + index;
    info->width = mode->hdisplay;
    info->height = mode->vdisplay;
    info->refresh = mode->vrefresh;
    info->interlace = mode->flags & DRM_MODE_FLAG_INTERLACE;
    info->ratio = 0;
    if (mode->flags & DRM_MODE_FLAG_PAR16_9)
        info->ratio = 2;
    else if (mode->flags & DRM_MODE_FLAG_PAR4_3)
        info->ratio = 1;

    ALOGI("Timing set is: %dx%d@%dHz",info->width, info->height, info->refresh);
}

bool drm_init()
{
    union drm_psb_extension_arg video_getparam_arg;
    const char video_getparam_ext[] = "lnc_video_getparam";

    memset(&gDrmCxt,0,sizeof(drmContext));
    gDrmCxt.hdmiConnector = NULL;
    gDrmCxt.drmFD = open(DRM_DEVICE_NAME, O_RDWR, 0);
    if (gDrmCxt.drmFD <= 0) {
        ALOGE("%s: Failed to open %s", __func__, DRM_DEVICE_NAME);
        return false;
    }

    strncpy(video_getparam_arg.extension,
            video_getparam_ext, sizeof(video_getparam_arg.extension));
    int ret = drmCommandWriteRead(gDrmCxt.drmFD, DRM_PSB_EXTENSION,
            &video_getparam_arg, sizeof(video_getparam_arg));
    if (ret != 0) {
        ALOGE("Failed to get ioctl offset.");
        drm_cleanup();
        return false;
    }

    gDrmCxt.ioctlOffset = video_getparam_arg.rep.driver_ioctl_offset;
    drmModeConnectorPtr connector = getConnector(gDrmCxt.drmFD, DRM_MODE_CONNECTOR_DVID);
    gDrmCxt.hdmiSupported = (connector != NULL);
    if (connector) {
        drmModeFreeConnector(connector);
        connector = NULL;
    }
    return true;
}

void drm_cleanup()
{
    if (gDrmCxt.drmFD > 0)
        drmClose(gDrmCxt.drmFD);
    if (gDrmCxt.hdmiConnector)
        drmModeFreeConnector(gDrmCxt.hdmiConnector);

    memset(&gDrmCxt, 0, sizeof(drmContext));
}

int drm_get_dev_fd()
{
    return gDrmCxt.drmFD;
}

int drm_get_ioctl_offset()
{
    return gDrmCxt.ioctlOffset;
}

bool drm_hdmi_isDeviceChanged(bool reset)
{
    bool ret = gDrmCxt.newDevice;
    if (reset)
        gDrmCxt.newDevice = false;
    return ret;
}

bool drm_hdmi_isSupported()
{
    return gDrmCxt.hdmiSupported;
}

bool drm_hdmi_setHdmiVideoOn()
{
    if (!gDrmCxt.hdmiSupported)
        return false;
    struct drm_psb_disp_ctrl dp_ctrl;
    memset(&dp_ctrl, 0, sizeof(dp_ctrl));
    dp_ctrl.cmd = DRM_PSB_DISP_PLANEB_ENABLE;
    dp_ctrl.u.data = HDMI_FORCE_VIDEO_ON_OFF;
    int ret = drmCommandWriteRead(gDrmCxt.drmFD,
                DRM_PSB_HDMI_FB_CMD, &dp_ctrl, sizeof(dp_ctrl));
    if (ret != 0) {
        ALOGE("%s failed, error = %d.", __func__, ret);
    }
    return ret == 0;
}

bool drm_hdmi_setHdmiVideoOff()
{
    if (!gDrmCxt.hdmiSupported)
        return false;
    struct drm_psb_disp_ctrl dp_ctrl;
    memset(&dp_ctrl, 0, sizeof(dp_ctrl));
    dp_ctrl.cmd = DRM_PSB_DISP_PLANEB_DISABLE;
    dp_ctrl.u.data = HDMI_FORCE_VIDEO_ON_OFF;
    int ret = drmCommandWriteRead(gDrmCxt.drmFD,
            DRM_PSB_HDMI_FB_CMD, &dp_ctrl, sizeof(dp_ctrl));
    if (ret != 0) {
        ALOGE("%s failed, error = %d.", __func__, ret);
    }
    return ret == 0;
}

bool drm_hdmi_setHdmiPowerOff()
{
    if (!gDrmCxt.hdmiSupported)
        return false;
    struct drm_psb_disp_ctrl dp_ctrl;
    memset(&dp_ctrl, 0, sizeof(dp_ctrl));
    dp_ctrl.cmd = DRM_PSB_HDMI_OSPM_ISLAND_DOWN;
    int ret = drmCommandWriteRead(gDrmCxt.drmFD,
            DRM_PSB_HDMI_FB_CMD, &dp_ctrl, sizeof(dp_ctrl));
    if (ret != 0) {
        ALOGE("%s failed, error = %d.", __func__, ret);
    }
    return ret == 0;
}

bool drm_hdmi_onHdmiDisconnected(void)
{
    gDrmCxt.connected = false;
    if (gDrmCxt.hdmiConnector)
        drmModeFreeConnector(gDrmCxt.hdmiConnector);
    gDrmCxt.hdmiConnector = NULL;
    return true;
}


bool drm_hdmi_notify_audio_hotplug(bool plugin)
{
    struct drm_psb_disp_ctrl dp_ctrl;
    memset(&dp_ctrl, 0, sizeof(dp_ctrl));
    dp_ctrl.cmd = DRM_PSB_HDMI_NOTIFY_HOTPLUG_TO_AUDIO;
    dp_ctrl.u.data = (plugin == true ? 1 : 0);
    int ret = drmCommandWriteRead(gDrmCxt.drmFD,
            DRM_PSB_HDMI_FB_CMD, &dp_ctrl, sizeof(dp_ctrl));
    return ret == 0;
}

// return 0 - not connected, 1 - HDMI connected, 2 - DVI connected
int drm_hdmi_getConnectionStatus()
{
    if (!gDrmCxt.hdmiSupported)
        return 0;

    if (gDrmCxt.hdmiConnector)
        drmModeFreeConnector(gDrmCxt.hdmiConnector);
    gDrmCxt.hdmiConnector = NULL;
    // reset connection status
    gDrmCxt.connected = false;
    drmModeConnector *connector = getHdmiConnector();
    if (connector == NULL)
        return 0;

    // Read EDID, and check whether it's HDMI or DVI interface
    int ret = 0, i, j;
    for (i = 0; i < connector->count_props; i++) {
        drmModePropertyPtr props = drmModeGetProperty(gDrmCxt.drmFD, connector->props[i]);
        if (!props)
            continue;

        if (props->name == NULL ||
                strncmp(props->name, "EDID", sizeof("EDID")) != 0) {
            drmModeFreeProperty(props);
            continue;
        }

        uint64_t* edid = &connector->prop_values[i];
        drmModePropertyBlobPtr edidBlob = drmModeGetPropertyBlob(gDrmCxt.drmFD, *edid);
        if (edidBlob == NULL ||
            edidBlob->data == NULL ||
            edidBlob->length < HDMI_TIMING_MAX) {
            ALOGE("%s: Invalid EDID Blob.", __func__);
            drmModeFreeProperty(props);
            ret = 0;
            break;
        }

        char* edid_binary = (char *)edidBlob->data;
        // offset of product_info
        char* product_info = edid_binary + 8;
        gDrmCxt.connected = true;
        if (memcmp(gDrmCxt.productInfo, product_info, EDID_PRODUCT_INFO_LEN)) {
            ALOGI("A new HDMI sink is connected.");
            gDrmCxt.newDevice = true;
            gDrmCxt.modeValid = false;
            memcpy(gDrmCxt.productInfo, product_info, EDID_PRODUCT_INFO_LEN);
        }

        drm_select_preferredmode(connector);

        ret = 2; // DVI
        if (edid_binary[126] == 0) {
            drmModeFreeProperty(props);
            break;
        }

        // search VSDB in extend edid
        for (j = 0; j <= HDMI_TIMING_MAX - 3; j++) {
            int n = HDMI_TIMING_MAX + j;
            if (edid_binary[n]   == 0x03 &&
                edid_binary[n+1] == 0x0c &&
                edid_binary[n+2] == 0x00) {
                ret = 1; //HDMI
                break;
            }
        }
        drmModeFreeProperty(props);
        break;
    }

    ALOGV("%s: connect status is %d", __func__, ret);
    return ret;
}


// return number of unique modes
int drm_hdmi_getModeInfo(
    int *pWidth,
    int *pHeight,
    int *pRefresh,
    int *pInterlace,
    int *pRatio)
{
    if (!gDrmCxt.hdmiSupported || !gDrmCxt.connected) {
        ALOGE("%s: HDMI is not supported or not connected.", __func__);
        return 0;
    }

    drmModeConnector *connector = getHdmiConnector();
    if (connector == NULL) {
        ALOGE("%s: Failed to get HDMI connector.", __func__);
        return 0;
    }

    if (connector->count_modes < 0 || connector->count_modes > HDMI_TIMING_MAX) {
        ALOGW("%s: unexpected count of modes %d", __func__, connector->count_modes);
        drmModeFreeConnector(connector);
        gDrmCxt.hdmiConnector = NULL;
        return 0;
    }

    int i, valid_mode_count = 0;
    // get resolution of each mode
    for (i = 0; i < connector->count_modes; i++) {
        unsigned int temp_hdisplay = connector->modes[i].hdisplay;
        unsigned int temp_vdisplay = connector->modes[i].vdisplay;
        unsigned int temp_refresh = connector->modes[i].vrefresh;
        // Only extract the required flags for comparison
        unsigned int temp_flags = connector->modes[i].flags &
          (DRM_MODE_FLAG_INTERLACE | DRM_MODE_FLAG_PAR16_9 | DRM_MODE_FLAG_PAR4_3);

        // re-traverse the connector mode list to see if there is
        // same resolution and refresh. The same mode will not be
        // counted into valid mode.

        int j = i;
        unsigned int flags = 0;

        while ((--j) >= 0) {
            flags = connector->modes[j].flags &
              (DRM_MODE_FLAG_INTERLACE | DRM_MODE_FLAG_PAR16_9 |
               DRM_MODE_FLAG_PAR4_3);

            if (temp_hdisplay == connector->modes[j].hdisplay &&
                temp_vdisplay == connector->modes[j].vdisplay &&
                temp_refresh == connector->modes[j].vrefresh &&
                temp_flags == flags) {
                  ALOGV("Found duplicated mode: %dx%d@%d with flags = 0x%x",
                       temp_hdisplay, temp_vdisplay, temp_refresh, temp_flags);
                  break;
            }
        }

        // if j<0, mode is not duplicated
        if (j < 0) {
            // pWidth, pHeight, etc can be NULL to get mode count.
            if (pWidth && pHeight && pRefresh && pInterlace && pRatio) {
                // record the valid mode info into mode array
                pWidth[valid_mode_count] = temp_hdisplay;
                pHeight[valid_mode_count] = temp_vdisplay;
                pRefresh[valid_mode_count] = temp_refresh;
                if (temp_flags & DRM_MODE_FLAG_INTERLACE)
                    pInterlace[valid_mode_count] = 1;
                else
                    pInterlace[valid_mode_count] = 0;
                if (temp_flags & DRM_MODE_FLAG_PAR16_9)
                    pRatio[valid_mode_count] = 2;
                else if (temp_flags & DRM_MODE_FLAG_PAR4_3)
                    pRatio[valid_mode_count] = 1;
                else
                    pRatio[valid_mode_count] = 0;

                ALOGV("Adding mode[%d]: %dx%d@%d with flags = 0x%x\n", valid_mode_count,
                     temp_hdisplay, temp_vdisplay, temp_refresh, temp_flags);
            }
            valid_mode_count++;
        }
    }

    return valid_mode_count;
}

bool drm_hdmi_setModeInfo(int width, int height, int refresh, int interlace, int ratio)
{
    if (!gDrmCxt.hdmiSupported || !gDrmCxt.connected) {
        ALOGE("%s: HDMI is not supported or not connected.", __func__);
        return false;
    }

    gDrmCxt.modeSelected.width = width;
    gDrmCxt.modeSelected.height = height;
    gDrmCxt.modeSelected.refresh = refresh;
    gDrmCxt.modeSelected.interlace = interlace;
    gDrmCxt.modeSelected.ratio = ratio;
    gDrmCxt.modeValid = true;

    ALOGI("User-selected mode is: %dx%d@%dHz, interlace = %d, ratio = %d",
        width, height, refresh, interlace, ratio);
    return true;
}

bool drm_hdmi_getTiming(int mode, MDSHDMITiming* info)
{
    if (info == NULL)
        return false;

    if (!gDrmCxt.hdmiSupported || !gDrmCxt.connected) {
        ALOGE("%s: HDMI is not supported or not connected.", __func__);
        return false;
    }

    ALOGV("Initial HDMI timing is %dx%d@%dHz",
            info->width, info->height, info->refresh);

    if (gDrmCxt.modeValid) {
        ALOGI("use user-selected mode.");
        memcpy(info, &gDrmCxt.modeSelected, sizeof(MDSHDMITiming));
        return true;
    }

    drmModeConnector *connector = getHdmiConnector();
    if (connector == NULL) {
        ALOGE("%s: get HDMI connector failed.", __func__);
        return false;
    }

    if (mode != DRM_HDMI_VIDEO_EXT) {
        drm_hdmi_setTiming(connector, gDrmCxt.preferredModeIndex, info);
        return true;
    }

    // Video extended mode, aspect ratio/interlace matching is not supported yet.
    // select 720P or 1080P as output
    if (info->width <= 1280 && info->height <= 720) {
        info->width = 1280;
        info->height = 720;
    } else {
        info->width = 1920;
        info->height = 1080;
    }

    // Find number of matched modes by resolution
    int i, num_matched = 0;
    int index_matched[HDMI_TIMING_MAX];
    for (i = 0; i < connector->count_modes; i++) {
        if (info->width == connector->modes[i].hdisplay &&
            info->height == connector->modes[i].vdisplay) {
            index_matched[num_matched++] = i;
        }
        if (num_matched >= HDMI_TIMING_MAX) {
            ALOGW("number of matched modes exceeds the limit.");
            break;
        }
    }

    if (num_matched == 0) {
        ALOGW("%s: Number of matched modes is 0.", __func__);
        // Use preferred mode
        drm_hdmi_setTiming(connector, gDrmCxt.preferredModeIndex, info);
        return true;
    }

    // select the best one from the matched modes
    int max_vrefresh_index = index_matched[0];
    int best_vrefresh_index = -1;
    drmModeModeInfoPtr modeInfo;
    for (i = 0; i < num_matched; i++) {
        modeInfo = connector->modes + index_matched[i];
        if (modeInfo->vrefresh > connector->modes[max_vrefresh_index].vrefresh) {
            max_vrefresh_index = index_matched[i];
        } else if (modeInfo->vrefresh == connector->modes[max_vrefresh_index].vrefresh) {
            if (drm_is_preferred_flags(modeInfo->flags)) {
                max_vrefresh_index = index_matched[i];
            }
        }
        if (info->refresh == 0)
            continue;
        if ((modeInfo->vrefresh % info->refresh) == 0) {
            if (modeInfo->vrefresh == info->refresh) {
                if (drm_is_preferred_flags(modeInfo->flags)) {
                    best_vrefresh_index = index_matched[i];
                    break;
                }
            } else if (best_vrefresh_index == -1 ||
                modeInfo->vrefresh > connector->modes[best_vrefresh_index].vrefresh) {
                best_vrefresh_index = index_matched[i];
            }
        }
    }

    if (best_vrefresh_index != -1) {
        drm_hdmi_setTiming(connector, best_vrefresh_index, info);
    } else {
        drm_hdmi_setTiming(connector, max_vrefresh_index, info);
    }
    return true;
}


bool drm_mipi_setMode(int mode)
{
    drmModeConnector *connector = getConnector(gDrmCxt.drmFD, DRM_MODE_CONNECTOR_MIPI);
    if (connector == NULL)
        return false;

    int i = 0;
    drmModePropertyPtr props = NULL;

    // Set MIPI On/Off
    for (i = 0; i < connector->count_props; i++) {
        props = drmModeGetProperty(gDrmCxt.drmFD, connector->props[i]);
        if (!props) continue;

        if (props->name != NULL &&
                !strncmp(props->name, "DPMS", sizeof("DPMS"))) {
            ALOGV("%s: %s %u", __func__,
                  (mode == DRM_MIPI_ON) ? "On" : "Off",
                  connector->connector_id);
            drmModeConnectorSetProperty(gDrmCxt.drmFD,
                                        connector->connector_id,
                                        props->prop_id,
                                        (mode == DRM_MIPI_ON)
                                        ? DRM_MODE_DPMS_ON : DRM_MODE_DPMS_OFF);
            drmModeFreeProperty(props);
            break;
        }
        drmModeFreeProperty(props);
    }

    drmModeFreeConnector(connector);
    return true;
}
