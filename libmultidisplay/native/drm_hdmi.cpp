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
#include <utils/Vector.h>
#ifndef VPG_DRM
#include "linux/psb_drm.h"
#endif
#include "drm_hdmi.h"
#include "xf86drm.h"
#include "xf86drmMode.h"

namespace android {
namespace intel {


#define EDID_PRODUCT_INFO_LEN   8
#define PREFERRED_VREFRESH      60  // 60Hz
#define DRM_DEVICE_NAME         "/dev/card0"


typedef struct _drmContext {
    int  drmFD;
    bool hdmiSupported;
    //bool newDevice;
    bool connected;
    int  preferredModeIndex;
    // The position of user selcected timing in Hdmi timings backup
    // and indicate user has selected a special timing
    int  selectedModeIndex;
#if 0 // Don't keep prevoius device EDID
    char productInfo[EDID_PRODUCT_INFO_LEN];
#endif
    Vector<MDSHdmiTiming*> hdmiTimings;
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
        if (connector->count_modes <= 0)
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
#ifndef VPG_DRM
        gDrmCxt.hdmiConnector = getConnector(gDrmCxt.drmFD, DRM_MODE_CONNECTOR_DVID);
#else
        gDrmCxt.hdmiConnector = getConnector(gDrmCxt.drmFD, DRM_MODE_CONNECTOR_HDMIA);
        if (gDrmCxt.hdmiConnector == NULL) {
            gDrmCxt.hdmiConnector = getConnector(gDrmCxt.drmFD, DRM_MODE_CONNECTOR_HDMIB);
        }
#endif
    if (gDrmCxt.hdmiConnector == NULL || gDrmCxt.hdmiConnector->modes == NULL) {
        ALOGW("Please check HDMI cable is connected or not");
        return NULL;
    }
    return gDrmCxt.hdmiConnector;
}

static inline bool drm_is_preferred_flags(unsigned int flags)
{
#ifndef VPG_DRM
    // prefer 16:9 over 4:3  and progressive over interlaced.
    if ((flags & DRM_MODE_FLAG_PAR16_9) && !(flags & DRM_MODE_FLAG_INTERLACE))
        return true;
    return false;
#else
    return true;
#endif
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
    ALOGI("HDMI preferred timing is: %dx%d@%dHz, index = %d",
            connector->modes[index_preferred].hdisplay,
            connector->modes[index_preferred].vdisplay,
            connector->modes[index_preferred].vrefresh,
            index_preferred);

}

static void addHdmiTimings(MDSHdmiTiming* dst) {
    MDSHdmiTiming* bak = new MDSHdmiTiming;
    memcpy(bak, dst, sizeof(MDSHdmiTiming));
    gDrmCxt.hdmiTimings.add(bak);
}

static void clearHdmiTimings() {
    gDrmCxt.selectedModeIndex = -1;
    for (unsigned int i = 0; i < gDrmCxt.hdmiTimings.size(); i++) {
        MDSHdmiTiming* timing = gDrmCxt.hdmiTimings.itemAt(i);
        delete timing;
        timing = NULL;
    }
    gDrmCxt.hdmiTimings.clear();
    ALOGV("Clear Hdmi Timings backup, %d", gDrmCxt.hdmiTimings.size());
}

bool drm_init()
{
    //gDrmCxt.newDevice = false;;
    gDrmCxt.connected = false;
    gDrmCxt.preferredModeIndex = -1;
    gDrmCxt.selectedModeIndex = -1;
    gDrmCxt.hdmiConnector = NULL;
#if 0 // Don't keep prevoius device EDID
    memset(gDrmCxt.productInfo, 0,EDID_PRODUCT_INFO_LEN);
#endif
#ifndef VPG_DRM
    gDrmCxt.drmFD = open(DRM_DEVICE_NAME, O_RDWR, 0);
    if (gDrmCxt.drmFD <= 0) {
        ALOGE("%s: Failed to open %s", __func__, DRM_DEVICE_NAME);
        return false;
    }
    drmModeConnectorPtr connector = getConnector(gDrmCxt.drmFD, DRM_MODE_CONNECTOR_DVID);
#else
    gDrmCxt.drmFD = drmOpen("i915", NULL);
    if (gDrmCxt.drmFD <= 0) {
        ALOGE("%s: Failed to open drm", __func__);
        return false;
    }
    drmModeConnectorPtr connector = getConnector(gDrmCxt.drmFD, DRM_MODE_CONNECTOR_HDMIA);
    if (connector == NULL) {
        connector = getConnector(gDrmCxt.drmFD, DRM_MODE_CONNECTOR_HDMIB);
    }
#endif
    gDrmCxt.hdmiSupported = (connector != NULL);
    if (connector) {
        drmModeFreeConnector(connector);
        connector = NULL;
    }
    gDrmCxt.hdmiTimings.setCapacity(HDMI_TIMING_MAX);
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

bool drm_hdmi_onHdmiDisconnected(void)
{
    clearHdmiTimings();
    gDrmCxt.connected = false;
    if (gDrmCxt.hdmiConnector)
        drmModeFreeConnector(gDrmCxt.hdmiConnector);
    gDrmCxt.hdmiConnector = NULL;
    return true;
}


bool drm_hdmi_notify_audio_hotplug(bool plugin)
{
#ifndef VPG_DRM
    struct drm_psb_disp_ctrl dp_ctrl;
    memset(&dp_ctrl, 0, sizeof(dp_ctrl));
    dp_ctrl.cmd = DRM_PSB_HDMI_NOTIFY_HOTPLUG_TO_AUDIO;
    dp_ctrl.u.data = (plugin == true ? 1 : 0);
    int ret = drmCommandWriteRead(gDrmCxt.drmFD,
            DRM_PSB_HDMI_FB_CMD, &dp_ctrl, sizeof(dp_ctrl));
    return ret == 0;
#else
    return true;
#endif
}

// return 0 - not connected, 1 - HDMI connected, 2 - DVI connected
int drm_hdmi_getConnectionStatus()
{
    ALOGV("Entering %s", __func__);
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
    int ret = 0;
#ifndef VPG_DRM
    // Read EDID, and check whether it's HDMI or DVI interface
    for (int i = 0; i < connector->count_props; i++) {
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
#if 0   // Don't keep prevoius device EDID
        gDrmCxt.newDevice = false;
        if (memcmp(gDrmCxt.productInfo, product_info, EDID_PRODUCT_INFO_LEN)) {
            ALOGI("A new HDMI sink is connected.");
            gDrmCxt.newDevice = true;
            memcpy(gDrmCxt.productInfo, product_info, EDID_PRODUCT_INFO_LEN);
            //clear HDMI timings backup
            clearHdmiTimings();
        }
#endif
        drm_select_preferredmode(connector);

        ret = 2; // DVI
        if (edid_binary[126] == 0) {
            drmModeFreeProperty(props);
            break;
        }

        // search VSDB in extend edid
        for (int j = 0; j <= HDMI_TIMING_MAX - 3; j++) {
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
#else
    if (connector->connection == DRM_MODE_CONNECTED) {
        gDrmCxt.connected = true;
        drm_select_preferredmode(connector);
        ret = 1; // Deault is HDMI on Gen
    }
#endif
    ALOGD("External Display device is %d", ret);
    return ret;
}

/*
 * Parse HDMI timings,
 * and save them in gDrmCxt.hdmiTimings
 */
static int parseHdmiTimings() {
    ALOGV("Entering %s", __func__);
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
    int validCnt = 0;
    // get resolution of each mode
    for (int i = 0; i < connector->count_modes; i++) {
        int tmpW = connector->modes[i].hdisplay;
        int tmpV = connector->modes[i].vdisplay;
        unsigned int tmpR = connector->modes[i].vrefresh;
        int tmpF = connector->modes[i].flags;
#ifdef VPG_DRM
        unsigned int tmpA = connector->modes[i].picture_aspect_ratio;
#endif
        bool duplicated = false;
        for (size_t j = 0; j < gDrmCxt.hdmiTimings.size(); j++) {
            MDSHdmiTiming* bak = gDrmCxt.hdmiTimings.itemAt(j);
            if (bak != NULL &&
                    bak->width == tmpW && bak->height == tmpV &&
#ifndef VPG_DRM
                    (bak->flags & DRM_MODE_FLAG_INTERLACE) == (tmpF & DRM_MODE_FLAG_INTERLACE) &&
                    (bak->flags & DRM_MODE_FLAG_PAR16_9) == (tmpF & DRM_MODE_FLAG_PAR16_9) &&
                    (bak->flags & DRM_MODE_FLAG_PAR4_3) == (tmpF & DRM_MODE_FLAG_PAR4_3) &&
                    bak->refresh == tmpR) {
#else
                    bak->refresh == tmpR && bak->flags == tmpF &&
                    bak->ratio == tmpA ) {
#endif
                duplicated = true;
                break;
            }
        }
        if (duplicated) {
            ALOGV("A duplicated timing:%dx%d@%dx%0xx", tmpW, tmpV, tmpR, tmpF);
            continue;
        }
        MDSHdmiTiming dst;
        dst.width   = tmpW;
        dst.height  = tmpV;
        dst.refresh = tmpR;
        dst.interlace = 0;
        if (tmpF & DRM_MODE_FLAG_INTERLACE)
            dst.interlace = 1;
        dst.ratio = 0;
#ifndef VPG_DRM
        if (tmpF & DRM_MODE_FLAG_PAR16_9)
            dst.ratio = 2;
        else if (tmpF & DRM_MODE_FLAG_PAR4_3)
            dst.ratio = 1;
#else
        if (tmpA == HDMI_PICTURE_ASPECT_16_9)
            dst.ratio = 2;
        else if (tmpA == HDMI_PICTURE_ASPECT_4_3)
            dst.ratio = 1;
#endif
        dst.flags = tmpF;
        // Save Hdmi timing
        addHdmiTimings(&dst);
        validCnt++;
        ALOGV("Add timing: %dx%d@%dx0x%0x", tmpW, tmpV, tmpR, tmpF);
    }
    return validCnt;
}

int drm_hdmi_getTimingNumber() {
    ALOGV("Entering %s", __func__);
    if (!gDrmCxt.hdmiSupported || !gDrmCxt.connected) {
        ALOGE("%s: HDMI is not supported or not connected.", __func__);
        return 0;
    }
    int number = gDrmCxt.hdmiTimings.size();
    if (number > 0)
        return number;
    return parseHdmiTimings();
}

// return number of unique modes
bool drm_hdmi_getTimings(int count, MDSHdmiTiming** list)
{
    ALOGV("Entering %s", __func__);
    if (!gDrmCxt.hdmiSupported || !gDrmCxt.connected) {
        ALOGE("%s: HDMI is not supported or not connected.", __func__);
        return false;
    }
    if (count <= 0 || list == NULL)
        return false;
    int validCnt = gDrmCxt.hdmiTimings.size();
    if (validCnt <= 0)
        validCnt = parseHdmiTimings();
    if (validCnt > count)
        validCnt = count;
    ALOGV("Hdmi timing number: %d, %d, %d", validCnt, count, gDrmCxt.hdmiTimings.size());
    for (int i = 0; i < validCnt; i++) {
        MDSHdmiTiming* bak = gDrmCxt.hdmiTimings.itemAt(i);
        MDSHdmiTiming* dst = *(list + i);
        if (dst != NULL)
            memcpy(dst, bak, sizeof(MDSHdmiTiming));
    }
    return true;
}

bool drm_hdmi_checkTiming(MDSHdmiTiming* timing)
{
    if (!timing || !gDrmCxt.hdmiSupported || !gDrmCxt.connected) {
        ALOGE("%s: HDMI is not supported or not connected.", __func__);
        return false;
    }
    unsigned int i = 0;
    unsigned int size = gDrmCxt.hdmiTimings.size();
    for (; i < size; i++) {
        MDSHdmiTiming* bak = gDrmCxt.hdmiTimings.itemAt(i);
        ALOGV("%dx%d@%dx%dx%d, %dx%d@%dx%dx%d",
                timing->width, timing->height,
                timing->refresh, timing->interlace, timing->ratio,
                bak->width, bak->height,
                bak->refresh, bak->interlace, bak->ratio);
        if (timing->width == bak->width &&
                timing->height == bak->height &&
                timing->refresh == bak->refresh &&
                timing->interlace == bak->interlace &&
                timing->ratio == bak->ratio) {
            timing->flags = bak->flags;
            gDrmCxt.selectedModeIndex = i;
            break;
        }
    }
    if (i >= size) {
        ALOGE("Fail to get a matched Hdmi timing, %d, %d", i, size);
        return false;
    }
    return true;
}
#if 0
bool drm_hdmi_isDeviceChanged()
{
    return gDrmCxt.newDevice;
}
#endif

bool drm_hdmi_timing_is_fixed()
{
    return (gDrmCxt.selectedModeIndex >= 0 ? true : false);
}

status_t drm_hdmi_get_current_timing(MDSHdmiTiming* timing)
{
    if (timing == NULL)
        return UNKNOWN_ERROR;
    int current = -1;
    MDSHdmiTiming* backup = NULL;
    ALOGV("Index %d, %d", gDrmCxt.selectedModeIndex, gDrmCxt.preferredModeIndex);
    if (gDrmCxt.selectedModeIndex >= 0)
        current = gDrmCxt.selectedModeIndex;
    else
        current = gDrmCxt.preferredModeIndex;
    if (current >= 0) {
        unsigned int size = gDrmCxt.hdmiTimings.size();
        if (size <= 0 || current >= size) {
            ALOGW("Wrong index %d, %d, Please check", current, size);
            return UNKNOWN_ERROR;
        }
        backup = gDrmCxt.hdmiTimings.itemAt(current);
        if (backup == NULL)
            return UNKNOWN_ERROR;
        memset(timing, 0, sizeof(MDSHdmiTiming));
        memcpy(timing, backup, sizeof(MDSHdmiTiming));
        return NO_ERROR;
    }
    return UNKNOWN_ERROR;
}

}; // namespace intel
}; // namespace android
