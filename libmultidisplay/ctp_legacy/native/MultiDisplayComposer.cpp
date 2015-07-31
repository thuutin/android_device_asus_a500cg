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

//#define LOG_NDEBUG 0

#include <utils/Log.h>
#include <binder/IServiceManager.h>
#include <binder/IPCThreadState.h>
#include <display/IMultiDisplayComposer.h>
#include <display/MultiDisplayComposer.h>
#include "drm_hdmi.h"
#include "drm_hdcp.h"

namespace android {
namespace intel {


#define MDC_CHECK_INIT() \
do { \
    if (mDrmInit == false) { \
        ALOGE("%s: drm_init fails", __func__); \
        return MDS_ERROR; \
    } \
} while(0)

MultiDisplayListener::MultiDisplayListener(int msg,
        const char* client, sp<IExtendDisplayListener> listener) {
    mMsg = msg;
    int len = strlen(client) + 1;
    mName = new char[len];
    strncpy(mName, client, len - 1);
    *(mName + len - 1) = '\0';
    ALOGV("%s: Register a new %s client, 0x%x", __func__, client, msg);
    mIEListener = listener;
}

MultiDisplayListener::~MultiDisplayListener() {
    ALOGV("%s: Unregister client %s", __func__, mName);
    mMsg = 0;
    delete [] mName;
    mName = NULL;
    mIEListener = NULL;
}

MultiDisplayComposer::MultiDisplayComposer() {
    mDrmInit = false;
    mMode = 0;
    mMipiPolicy = MDS_MIPI_OFF_NOT_ALLOWED;
    mHdmiPolicy = MDS_HDMI_ON_ALLOWED;
    memset((void*)(&mVideo), 0, sizeof(MDSVideoSourceInfo));
    mMipiOn = true;
    mWidiVideoExt = false;
    mMipiReq = NO_MIPI_REQ;
    mSurfaceComposer = NULL;
    mScaleMode = 0;
    mScaleStepX = 0;
    mScaleStepY = 0;
    mConnectStatus = 0;

    // Default value
    mDisplayCapability = MDS_HW_SUPPORT_WIDI;

    initialize_l();
}

MultiDisplayComposer::~MultiDisplayComposer() {
    drm_cleanup();
    if (!mListener.isEmpty()) {
        mListener.clear();
    }
}

int MultiDisplayComposer::getMode(bool wait) {
    MDC_CHECK_INIT();
    if (wait)
        mLock.lock();
    else {
        if (mLock.tryLock() == -EBUSY) {
            //ALOGW("%s: couldn't hold lock", __func__);
            return MDS_MODE_NONE;
        }
    }
    int mode = mMode;
    mLock.unlock();
    return mode;
}

int MultiDisplayComposer::notifyWidi(bool on) {
    MDC_CHECK_INIT();
    Mutex::Autolock _l(mMipiLock);
    mWidiVideoExt = on;
    if (mWidiVideoExt)
        mMode |= MDS_WIDI_ON;
    else
        mMode &= ~MDS_WIDI_ON;

    return MDS_NO_ERROR;
}

int MultiDisplayComposer::notifyMipi(bool on) {
    MDC_CHECK_INIT();
    Mutex::Autolock _l(mMipiLock);
    mMipiReq = on ? MIPI_ON_REQ : MIPI_OFF_REQ;
    mMipiCon.signal();
    return MDS_NO_ERROR;
}

int MultiDisplayComposer::setHdmiPowerOff() {
    MDC_CHECK_INIT();
    Mutex::Autolock _l(mLock);
    drm_hdmi_setHdmiPowerOff();
    return MDS_NO_ERROR;
}

int MultiDisplayComposer::prepareForVideo(int status) {
    MDC_CHECK_INIT();
    Mutex::Autolock _l(mLock);
    if (mVideoState == status)
        return MDS_NO_ERROR;
    ALOGV("%s: Video status %d", __func__, status);
    mVideoState = status;
    broadcastMessage_l(MDS_SET_VIDEO_STATUS, &status, sizeof(status));
    if (status == (int)MDS_VIDEO_UNPREPARED) {
        MDSVideoSourceInfo info;
        memset(&mVideo, 0, sizeof(info));
        return setHdmiMode_l();
    }
    return MDS_NO_ERROR;
}

int MultiDisplayComposer::getVideoState() {
    MDC_CHECK_INIT();
    Mutex::Autolock _l(mLock);
    return mVideoState;
}

int MultiDisplayComposer::updateVideoInfo(const MDSVideoSourceInfo& info) {
    MDC_CHECK_INIT();
    Mutex::Autolock _l(mLock);
    ALOGV("update video info: \
        \n mode: 0x%x, \
        \n is playing: %d, \
        \n is protected Content: %d, \
        \n displayW: %d, \
        \n displayH: %d, \
        \n frameRate: %d, \
        \n is Interlaced: %d",
        mMode, info.isPlaying, info.isProtected,
        info.displayW, info.displayH, info.frameRate, info.isInterlaced);

    memcpy(&mVideo, &info, sizeof(MDSVideoSourceInfo));

    return setHdmiMode_l();
}


int MultiDisplayComposer::setHdmiMode_l() {
    ALOGV("Entering %s, current mode = %#x", __func__, mMode);
    MDSHDMITiming timing;
    memset(&timing, 0, sizeof(MDSHDMITiming));

    // Common case, update video status
    if (mVideo.isPlaying) {
        mMode |= MDS_VIDEO_PLAYING;
    } else {
        mMode &= ~MDS_VIDEO_PLAYING;
    }
    // Common case, check HDMI connect status
    int connectStatus = getHdmiPlug_l();
#if !defined(DVI_SUPPORTED)
    if (connectStatus == DRM_DVI_CONNECTED) {
        ALOGE("%s: DVI is connected but is not supported for now.", __func__);
        broadcastMessage_l(MDS_MODE_CHANGE, &mMode, sizeof(mMode));
        mConnectStatus = connectStatus;
        return MDS_ERROR;
    }
#endif
    // Common case, turn on MIPI if necessary
    if (connectStatus == DRM_HDMI_DISCONNECTED ||
        mHdmiPolicy == MDS_HDMI_ON_NOT_ALLOWED ||
        mVideo.isPlaying == false) {
        mMipiPolicy = MDS_MIPI_OFF_NOT_ALLOWED;
        if (!checkMode(mMode, MDS_MIPI_ON)) {
            ALOGI("Turn on MIPI.");
            drm_mipi_setMode(DRM_MIPI_ON);
            mMode |= MDS_MIPI_ON;
            mMipiOn = true;
        }
    }
    // Common case, HDMI is disconnected
    if (connectStatus == DRM_HDMI_DISCONNECTED) {
        ALOGI("HDMI is disconnected.");
        if (!checkMode(mMode, MDS_HDMI_CONNECTED)) {
            ALOGW("HDMI is already in disconnected state.");
            broadcastMessage_l(MDS_MODE_CHANGE, &mMode, sizeof(mMode));
            mConnectStatus = connectStatus;
            return MDS_NO_ERROR;
        }

        ALOGV("Notify HDMI audio driver hot unplug event.");
        drm_hdmi_notify_audio_hotplug(false);
        drm_hdcp_disable_hdcp(false);
        mMode &= ~MDS_HDMI_CONNECTED;
        mMode &= ~MDS_HDMI_ON;
        mMode &= ~MDS_HDCP_ON;
        mMode &= ~MDS_HDMI_CLONE;
        mMode &= ~MDS_HDMI_VIDEO_EXT;
        mMode &= ~MDS_OVERLAY_OFF;
        broadcastMessage_l(MDS_MODE_CHANGE, &mMode, sizeof(mMode));
        drm_hdmi_onHdmiDisconnected();
        mConnectStatus = connectStatus;
        return MDS_NO_ERROR;
    }

    ALOGI("HDMI is connected.");
    bool notify_audio_hotplug = (mMode & MDS_HDMI_CONNECTED) == 0;
    mMode |= MDS_HDMI_CONNECTED;

    // Common case, check HDMI policy
    if (mHdmiPolicy == MDS_HDMI_ON_NOT_ALLOWED) {
        ALOGI("HDMI on is not allowed. Turning off HDMI...");
        if (mConnectStatus != connectStatus &&
               notify_audio_hotplug && mDrmInit) {
            // Do not need to notify HDMI audio driver about hotplug during startup.
            ALOGV("Notify HDMI audio drvier hot plug event.");
            drm_hdmi_notify_audio_hotplug(true);
            mConnectStatus = connectStatus;
        }
        if (!checkMode(mMode, MDS_HDMI_ON)) {
            ALOGW("HDMI is already in off state.");
            broadcastMessage_l(MDS_MODE_CHANGE, &mMode, sizeof(mMode));
            return MDS_NO_ERROR;
        }
        if (checkMode(mMode, MDS_HDCP_ON)) {
            drm_hdcp_disable_hdcp(true);
        }
        mMode &= ~MDS_HDCP_ON;
        mMode &= ~MDS_HDMI_ON;
        mMode &= ~MDS_HDMI_CLONE;
        mMode &= ~MDS_HDMI_VIDEO_EXT;
        broadcastMessage_l(MDS_MODE_CHANGE, &mMode, sizeof(mMode));
        drm_hdmi_setHdmiVideoOff();
        return MDS_NO_ERROR;
    }
    mConnectStatus = connectStatus;

    if (mVideo.isPlaying && checkMode(mMode, MDS_HDMI_VIDEO_EXT)) {
        ALOGW("HDMI is already in Video Extended mode.");
        broadcastMessage_l(MDS_MODE_CHANGE, &mMode, sizeof(mMode));
        return MDS_NO_ERROR;
    } else if (!mVideo.isPlaying && checkMode(mMode, MDS_HDMI_CLONE)) {
        ALOGW("HDMI is already in cloned state.");
        broadcastMessage_l(MDS_MODE_CHANGE, &mMode, sizeof(mMode));
        return MDS_NO_ERROR;
    }
    // Common case, turn off overlay temporarily during mode transition.
    // Make sure overlay is turned on when this function exits.
    // Transition mode starts with standalone local mipi mode (no cloned, no video extended).
    int transitionalMode = mMode;
    transitionalMode &= ~MDS_HDMI_CLONE;
    transitionalMode &= ~MDS_HDMI_VIDEO_EXT;
    transitionalMode |= MDS_OVERLAY_OFF;
    broadcastMessage_l(MDS_MODE_CHANGE, &transitionalMode, sizeof(transitionalMode));

    //Before HDMI mode change, disalbe HDCP
    if (checkMode(mMode, MDS_HDCP_ON)) {
        ALOGV("Turning off HDCP before mode change");
        drm_hdcp_disable_hdcp(true);
        mMode &= ~MDS_HDCP_ON;
    }

    // Common case, notify HWC to set HDMI timing if need
    if (mVideo.isPlaying) {
        // disable dynamic mode setting for presentation mode
        if (isHdmiTimingDynamicSettingEnable_l()) {
            ALOGI("Video is in playing state. Mode = %#x", mMode);
            timing.width = mVideo.displayW;
            timing.height = mVideo.displayH;
            timing.refresh = mVideo.frameRate;
            timing.interlace = 0;
            timing.ratio = 0;
            drm_hdmi_getTiming(DRM_HDMI_VIDEO_EXT, &timing);
            setHdmiTiming_l((void*)&timing, sizeof(MDSHDMITiming));
        }
    } else {
        ALOGV("Video is not in playing state. Mode = %#x", mMode);
        drm_hdmi_getTiming(DRM_HDMI_CLONE, &timing);
        setHdmiTiming_l((void*)&timing, sizeof(MDSHDMITiming));
    }
    // Common case, turn on HDCP
    if (drm_hdcp_enable_hdcp() == false) {
        ALOGE("Fail to enable HDCP.");
        // Continue mode setting as it may be recovered, unless HDCP is not supported.
        // If HDCP is not supported, user will have to unplug the cable to restore video to phone.
    } else {
        ALOGV("Turning on HDCP...");
        mMode |= MDS_HDCP_ON;
    }
    // Common case, prolong overlay off time
    mMode |= MDS_OVERLAY_OFF;
    if (mVideo.isPlaying) {
        mMode |= MDS_HDMI_VIDEO_EXT;
        mMode &= ~MDS_HDMI_CLONE;
        mMipiPolicy = MDS_MIPI_OFF_ALLOWED;
    } else {
        mMode &= ~MDS_HDMI_VIDEO_EXT;
        mMode |= MDS_HDMI_CLONE;
    }
    broadcastMessage_l(MDS_MODE_CHANGE, &mMode, sizeof(mMode));

    // Common case, turn on HDMI if need
    if (!checkMode(mMode, MDS_HDMI_ON)) {
        ALOGI("Turn on HDMI...");
        if (!drm_hdmi_setHdmiVideoOn()) {
            ALOGI("Fail to turn on HDMI.");
        }
        mMode |= MDS_HDMI_ON;
        broadcastMessage_l(MDS_MODE_CHANGE, &mMode, sizeof(mMode));
    }

    // Common case, notify HWC to turn on Overlay if need
    if (checkMode(mMode, MDS_HDMI_VIDEO_EXT)) {
        //Enable overlay lastly
        mMode &= ~MDS_OVERLAY_OFF;
    }
    broadcastMessage_l(MDS_MODE_CHANGE, &mMode, sizeof(mMode));

    // Common case, notify audio driver if need
    if (notify_audio_hotplug && mDrmInit) {
        // Do not need to notify HDMI audio driver about hotplug during startup.
        ALOGV("Notify HDMI audio drvier hot plug event.");
        drm_hdmi_notify_audio_hotplug(true);
    }

    ALOGV("Leaving %s, new mode is %#x", __func__, mMode);
    return MDS_NO_ERROR;
}

void MultiDisplayComposer::broadcastMessage_l(int msg, void* value, int size) {
    for (unsigned int index = 0; index < mListener.size(); index++) {
        MultiDisplayListener* listener = mListener.valueAt(index);
        if (listener == NULL)
            continue;
        if (listener->getName() != NULL)
            ALOGV("%s: Broadcast message 0x%x to %s, 0x%x", __func__, msg, listener->getName(), *((int*)value));
        if (listener->checkMsg(msg)) {
            sp<IExtendDisplayListener> ielistener = listener->getIEListener();
            if (ielistener != NULL)
                ielistener->onMdsMessage(msg, value, size);
        }
    }
}

int MultiDisplayComposer::setHdmiTiming_l(void* value, int size) {
    int ret = MDS_ERROR;
    sp<IExtendDisplayListener> ielistener = NULL;
    bool hasHwc = false;
    for (unsigned int index = 0; index < mListener.size(); index++) {
        MultiDisplayListener* listener = mListener.valueAt(index);
        if (listener == NULL)
            continue;
        char* client = listener->getName();
        if (client && !strncmp(client, "HWComposer", sizeof("HWComposer"))) {
            hasHwc = true;
            if (listener->checkMsg(MDS_SET_TIMING)) {
                ALOGV("%s: Set HDMI timing through HWC", __func__);
                ielistener = listener->getIEListener();
                break;
            }
        }
    }
    if (ielistener != NULL)
        ret = ielistener->onMdsMessage(MDS_SET_TIMING, value, size);
    else if (!hasHwc)
        ret = MDS_NO_ERROR;
    return ret;
}

int MultiDisplayComposer::setMipiMode_l(bool on) {
    Mutex::Autolock _l(mLock);

    if (mMipiOn == on)
        return MDS_NO_ERROR;

    if (on) {
        drm_mipi_setMode(DRM_MIPI_ON);
        mMode |= MDS_MIPI_ON;
    } else {
        if(!mWidiVideoExt) {
            if (!checkMode(mMode, MDS_HDMI_VIDEO_EXT)) {
                ALOGW("%s: Attempt to turn off Mipi while not in extended video mode.", __func__);
                broadcastMessage_l(MDS_MODE_CHANGE, &mMode, sizeof(mMode));
                return MDS_ERROR;
            }
            if (mMipiPolicy == MDS_MIPI_OFF_ALLOWED) {
                drm_mipi_setMode(DRM_MIPI_OFF);
                mMode &= ~MDS_MIPI_ON;
            }
        } else {
            drm_mipi_setMode(DRM_MIPI_OFF);
            mMode &= ~MDS_MIPI_ON;
        }
    }
    mMipiOn = on;
    ALOGI("Turn on/off mipi, %d", on);
    return MDS_NO_ERROR;
}

int MultiDisplayComposer::notifyHotPlug() {
    int ret = MDS_ERROR;
    MDC_CHECK_INIT();
    ALOGV("%s: mipi policy: %d, hdmi policy: %d, mode: 0x%x", __func__, mMipiPolicy, mHdmiPolicy, mMode);
    Mutex::Autolock _l(mLock);
    ret = setHdmiMode_l();
    if (drm_hdmi_isDeviceChanged(false) && mConnectStatus == DRM_HDMI_CONNECTED)
        setDisplayScalingLocked(0, 0, 0);
    return ret;
}

int MultiDisplayComposer::setModePolicy(int policy) {
    MDC_CHECK_INIT();
    Mutex::Autolock _l(mLock);
    return setModePolicy_l(policy);
}

int MultiDisplayComposer::setModePolicy_l(int policy) {
    int ret = 0;
    unsigned int index = 0;
    ALOGI("%s: policy %d, mHdmiPolicy 0x%x, mMipiPolicy 0x%x, mMode: 0x%x",
            __func__, policy, mHdmiPolicy, mMipiPolicy, mMode);
    switch (policy) {
        case MDS_HDMI_ON_NOT_ALLOWED:
        case MDS_HDMI_ON_ALLOWED:
            mHdmiPolicy = policy;
            return setHdmiMode_l();

        case MDS_MIPI_OFF_NOT_ALLOWED:
            mMipiPolicy = policy;
            if (!checkMode(mMode, MDS_MIPI_ON)) {
                drm_mipi_setMode(DRM_MIPI_ON);
            }
            mMode |= MDS_MIPI_ON;
            mMipiOn = true;
            break;
        case MDS_MIPI_OFF_ALLOWED:
            mMipiPolicy = policy;
            if (checkMode(mMode, MDS_MIPI_ON)) {
                drm_mipi_setMode(DRM_MIPI_OFF);
            }
            mMode &= ~MDS_MIPI_ON;
            mMipiOn = false;
            break;
        default:
            return MDS_ERROR;
    }
    broadcastMessage_l(MDS_MODE_CHANGE, &mMode, sizeof(mMode));
    return MDS_NO_ERROR;
}

int MultiDisplayComposer::registerListener(
                sp<IExtendDisplayListener> listener,
                void *handle, const char* name, int msg) {
    unsigned int i = 0;
    MDC_CHECK_INIT();
    if (name == NULL || msg == 0) {
        ALOGE("%s: Failed to register a no-name or no-message client", __func__);
        return MDS_ERROR;
    }
    Mutex::Autolock _l(mLock);
    for (i = 0; i < mListener.size(); i++) {
        if (mListener.keyAt(i) == handle) {
            ALOGE("%s register error!", __func__);
            return MDS_ERROR;
        }
    }
    MultiDisplayListener* tlistener = new  MultiDisplayListener(msg, name, listener);
    mListener.add(handle, tlistener);
    return MDS_NO_ERROR;
}

int MultiDisplayComposer::unregisterListener(void *handle) {
    unsigned int i = 0;
    MDC_CHECK_INIT();
    Mutex::Autolock _l(mLock);
    for (i = 0; i < mListener.size(); i++) {
        if (mListener.keyAt(i) == handle) {
            MultiDisplayListener* tlistener = mListener.valueAt(i);
            mListener.removeItem(handle);
            delete tlistener;
            tlistener = NULL;
            break;
        }
    }
    return MDS_NO_ERROR;
}

int MultiDisplayComposer::getHdmiPlug_l() {
    return drm_hdmi_getConnectionStatus();
}

int MultiDisplayComposer::getHdmiModeInfo(int* pWidth, int* pHeight,
                                          int* pRefresh, int* pInterlace,
                                          int *pRatio) {
    MDC_CHECK_INIT();
    Mutex::Autolock _l(mLock);
    ALOGV("%s: mMode: 0x%x", __func__, mMode);
    if (pWidth == NULL || pHeight == NULL ||
            pRefresh == NULL || pInterlace == NULL) {
      return drm_hdmi_getModeInfo(NULL, NULL, NULL, NULL, NULL);
    }
    return drm_hdmi_getModeInfo(pWidth, pHeight, pRefresh, pInterlace, pRatio);
}

int MultiDisplayComposer::setHdmiModeInfo(int width, int height,
                            int refresh, int interlace, int ratio) {
    MDSHDMITiming timing;
    MDC_CHECK_INIT();
    Mutex::Autolock _l(mLock);
    ALOGV("%s: \
        \n  mMode: %d, \
        \n  width: %d, \
        \n  height: %d, \
        \n  refresh: %d, \
        \n  ratio: %d, \
        \n  interlace: %d",
         __func__, mMode, width, height, refresh, ratio, interlace);

    drm_hdmi_setModeInfo(width, height, refresh, interlace, ratio);
    drm_hdmi_getTiming(DRM_HDMI_CLONE, &timing);
    setHdmiTiming_l((void*)&timing, sizeof(MDSHDMITiming));
    broadcastMessage_l(MDS_MODE_CHANGE, &mMode, sizeof(mMode));
    return MDS_NO_ERROR;
}

int MultiDisplayComposer::setDisplayScalingLocked(uint32_t mode,
         uint32_t stepx, uint32_t stepy) {
    if (mSurfaceComposer == NULL) {
        const sp<IServiceManager> sm = defaultServiceManager();
        const String16 name("SurfaceFlinger");
        mSurfaceComposer = sm->getService(name);
        if (mSurfaceComposer == NULL) {
            return -1;
        }
    }

    uint32_t scale;
    Parcel data, reply;
    const String16 token("android.ui.ISurfaceComposer");

    scale = mode | stepx << 16 | stepy << 24;
    data.writeInterfaceToken(token);
    data.writeInt32(scale);
    mSurfaceComposer->transact(SFIntelHDMIScalingSetting, data, &reply);
    return reply.readInt32();
}

int MultiDisplayComposer::setHdmiScaleType(int type) {
    MDC_CHECK_INIT();
    Mutex::Autolock _l(mLock);

    mScaleMode = type;
    return setDisplayScalingLocked(mScaleMode, mScaleStepX, mScaleStepY);
}

int MultiDisplayComposer::setHdmiScaleStep(int hValue, int vValue) {
    MDC_CHECK_INIT();
    Mutex::Autolock _l(mLock);

    mScaleStepX = (hValue > 5) ? 0: (5 - hValue);
    mScaleStepY = (vValue > 5) ? 0: (5 - vValue);
    return setDisplayScalingLocked(mScaleMode, mScaleStepX, mScaleStepY);
}

int MultiDisplayComposer::getHdmiDeviceChange() {
    MDC_CHECK_INIT();
    Mutex::Autolock _l(mLock);
    return drm_hdmi_isDeviceChanged(true);
}

int MultiDisplayComposer::getVideoInfo(int* dw, int* dh, int* fps, int* interlaced) {
    MDC_CHECK_INIT();
    //TODO: here don't need to lock mLock
    if (!mVideo.isPlaying) {
        ALOGE("%s: Video player is not in playing state", __func__);
        return MDS_ERROR;
    }
    if (dw)
        *dw = mVideo.displayW;
    if (dh)
        *dh = mVideo.displayH;
    if (fps)
        *fps = mVideo.frameRate;
    if (interlaced)
        *interlaced = mVideo.isInterlaced;
    return MDS_NO_ERROR;
}

void MultiDisplayComposer::initialize_l() {
    if (!drm_init()) {
        ALOGE("%s: drm_init fails.", __func__);
        return;
    }
    mDrmInit = true;
    if (drm_hdmi_isSupported()) {
        mDisplayCapability |= MDS_HW_SUPPORT_HDMI;
        setModePolicy_l(MDS_HDMI_ON_ALLOWED);
    }
    setModePolicy_l(MDS_MIPI_OFF_NOT_ALLOWED);

    // start mipi listener
    run("MIPIListener", PRIORITY_URGENT_DISPLAY);
}

int MultiDisplayComposer::getDisplayCapability() {
    return mDisplayCapability;
}

bool MultiDisplayComposer::threadLoop() {
    bool mipiOn;
    while(true) {
        {
            Mutex::Autolock _l(mMipiLock);
            if (mMipiReq == NO_MIPI_REQ)
                mMipiCon.wait(mMipiLock);
            mipiOn = (mMipiReq == MIPI_ON_REQ) ? true : false;
            mMipiReq = NO_MIPI_REQ;
        }
        setMipiMode_l(mipiOn);
    }
    return MDS_NO_ERROR;
}

bool MultiDisplayComposer::isHdmiTimingDynamicSettingEnable_l() {
    // Always enable dynamic setting for video playback
    return true;
    // Presentation mode checking depends on GFX SF patch,
    // this patch hasn't been ported
    if (mSurfaceComposer == NULL) {
        const sp<IServiceManager> sm = defaultServiceManager();
        const String16 name("SurfaceFlinger");
        mSurfaceComposer = sm->getService(name);
        if (mSurfaceComposer == NULL) {
            return true;
        }
    }

    Parcel data, reply;
    int ret = -1;
    const String16 token("android.ui.ISurfaceComposer");
    data.writeInterfaceToken(token);
    status_t status = mSurfaceComposer->transact(SFIntelQueryPresentationMode, data, &reply);
    if (status == NO_ERROR)
        ret = reply.readInt32();
    ALOGI("Enable HDMI Timing dynamic setting %d", ret);
    return (ret ? false : true);
}

}; // namespace intel
}; // namespace android
