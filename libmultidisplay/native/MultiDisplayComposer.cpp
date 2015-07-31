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
#include <utils/RefBase.h>
#include <binder/Parcel.h>

#include <binder/IServiceManager.h>
#include <binder/IPCThreadState.h>
#include "MultiDisplayComposer.h"
#include "drm_hdmi.h"
#ifdef TARGET_HAS_ISV
#include "VPPSetting.h"
#endif

namespace android {
namespace intel {

#define MDC_CHECK_INIT() \
do { \
    if (mDrmInit == false) { \
        ALOGE("%s: drm_init fails", __func__); \
        return NO_INIT; \
    } \
} while(0)

#define CHECK_VIDEO_SESSION_ID(SESSIONID, ERR) \
do { \
    if (SESSIONID < 0 || SESSIONID >= MDS_VIDEO_SESSION_MAX_VALUE) { \
        ALOGE("Invalid session ID %d", SESSIONID); \
        return ERR; \
    } \
} while(0)


MultiDisplayListener::MultiDisplayListener(int msg, int32_t id,
        const char* client, sp<IMultiDisplayListener> listener) {
    mMsg  = msg;
    mId   = id;
    mName = new String8(client);
    mListener = listener;
}

MultiDisplayListener::~MultiDisplayListener() {
    mMsg = 0;
    mId = -1;
    delete mName;
    mName = NULL;
    mListener = NULL;
}

void MultiDisplayListener::dump() {
    if (mName == NULL) {
        ALOGE("Error listener");
        return;
    }
    ALOGV("Listener info: %d, %d, %s, %p",
            mMsg, mId, mName->string(), mListener.get());
}


void MultiDisplayVideoSession::dump(int index) {
    if (mState < MDS_VIDEO_PREPARING ||
            mState >= MDS_VIDEO_UNPREPARED)
        return;
    ALOGV("\t[%d] %d, %dx%d@%d", index, mState,
            mInfo.displayW, mInfo.displayH, mInfo.frameRate);
}

MultiDisplayComposer::MultiDisplayComposer() :
    mDrmInit(false),
#ifdef TARGET_HAS_ISV
    mDisplayId(MDS_DISPLAY_PRIMARY),
#endif
    mListenerId(0),
    mMode(MDS_MODE_NONE),
    mScaleType(MDS_SCALING_NONE),
    mHorizontalStep(0),
    mVerticalStep(0),
    mSurfaceComposer(NULL),
    mMDSCallback(NULL)
{
    init();
}

MultiDisplayComposer::~MultiDisplayComposer() {
    drm_cleanup();

    // Remove all the listeners.
    size_t size = mListeners.size();
    if (size > 0) {
        MultiDisplayListener* listener = NULL;
        for (size_t i = 0; i < size; i++) {
            listener = mListeners.valueAt(i);
            delete listener;
        }
        mListeners.clear();
    }

    mSurfaceComposer = NULL;
    mMDSCallback = NULL;
}

void MultiDisplayComposer::init() {
    if (!drm_init()) {
        ALOGE("Fail to init drm");
        return;
    }

    mDrmInit = true;

    initVideoSessions_l();
    updateHdmiConnectStatusLocked();
    // TODO: if HDMI is connected, update vpp policy
    //setDisplayState_l(MDS_DISPLAY_EXTERNAL, VPPSetting::isVppOn());
}

status_t MultiDisplayComposer::updateHdmiConnectStatusLocked() {
    MDC_CHECK_INIT();

    int connectStatus = drm_hdmi_getConnectionStatus();
    if (connectStatus == DRM_HDMI_CONNECTED) {
        mMode |= MDS_HDMI_CONNECTED;
    } else if (connectStatus == DRM_DVI_CONNECTED) {
        mMode |= MDS_DVI_CONNECTED;
    } else {
        mMode &= ~(MDS_HDMI_CONNECTED | MDS_DVI_CONNECTED);
        drm_hdmi_onHdmiDisconnected();
    }
    ALOGI("ConnectStatus is %d, mode is 0x%x", connectStatus, mMode);
    return NO_ERROR;
}

status_t MultiDisplayComposer::registerCallback(const sp<IMultiDisplayCallback>& cbk) {
    Mutex::Autolock lock(mMutex);
    if (cbk.get() == NULL) {
        ALOGE("Callback is null");
        return BAD_VALUE;
    }
    mMDSCallback = cbk;

    // Make sure the hdmi status is aligned
    // between MDS and hwc.
    updateHdmiConnectStatusLocked();
    return NO_ERROR;
}

status_t MultiDisplayComposer::unregisterCallback(const sp<IMultiDisplayCallback>& cbk) {
    Mutex::Autolock lock(mMutex);
    mMDSCallback = NULL;
    return NO_ERROR;
}

status_t MultiDisplayComposer::updateHdmiConnectionStatus(bool connected) {
    Mutex::Autolock lock(mMutex);
    return notifyHotplugLocked(MDS_DISPLAY_EXTERNAL, connected);
}

status_t MultiDisplayComposer::updateWidiConnectionStatus(bool connected) {
    Mutex::Autolock lock(mMutex);
    return notifyHotplugLocked(MDS_DISPLAY_VIRTUAL, connected);
}

status_t MultiDisplayComposer::notifyHotplugLocked(
        MDS_DISPLAY_ID dispId, bool connected) {
    ALOGI("Display ID:%d, connected state:%d", dispId, connected);
    // Notify widi video extended mode
    int mode = mMode;
    // update vpp policy
    //setVppState_l(dispId, connected);
    if (dispId == MDS_DISPLAY_VIRTUAL) {
        if (connected)
            mMode |= MDS_WIDI_ON;
        else
            mMode &= ~MDS_WIDI_ON;
        broadcastMessageLocked((int)MDS_MSG_MODE_CHANGE, &mMode, sizeof(mMode), false);
        return NO_ERROR;
    }
    // Notify hdmi hotplug and switch audio
    if (hasVideoPlaying_l()) {
        mMode |= MDS_VIDEO_ON;
    } else {
        mMode &= ~MDS_VIDEO_ON;
    }
    updateHdmiConnectStatusLocked();

    if (mode != mMode) {
        int connection = connected ? 1 : 0;
        broadcastMessageLocked((int)MDS_MSG_MODE_CHANGE, &mMode, sizeof(mMode), false);
        drm_hdmi_notify_audio_hotplug(connected);
    }
    // set oversan compensation and scaling type
    status_t result = UNKNOWN_ERROR;
    // Check the callback implementation
    if (mMDSCallback != NULL) {
        if (mScaleType != MDS_SCALING_NONE)
            result = mMDSCallback->setHdmiScalingType(MDS_SCALING_NONE);
        if (result == NO_ERROR &&
                (mHorizontalStep != 0 || mVerticalStep != 0))
            result = mMDSCallback->setHdmiOverscan(0, 0);
    }
    // If not implemented in callback, call SurfaceFlinger directly!
    if (result != NO_ERROR) {
        if (mScaleType != MDS_SCALING_NONE ||
                mHorizontalStep != 0 || mVerticalStep != 0)
            result = setDisplayScalingLocked(MDS_SCALING_NONE, 0, 0);
    }

    if (result == NO_ERROR) {
        mScaleType = MDS_SCALING_NONE;
        mHorizontalStep = 0;
        mVerticalStep = 0;
    }

    return NO_ERROR;
}

status_t MultiDisplayComposer::updateVideoState(int sessionId, MDS_VIDEO_STATE state) {
    Mutex::Autolock lock(mMutex);
    status_t result = NO_ERROR;
    //FIXME: Video user space driver works at different process,
    // When MDS receive a UNPREPARING or UNPREPARED state,
    // video driver may has been unloaded,
    // if MDS still broadcast unprepared message to video driver,
    // it may cause a Binder erorr.
    bool ignoreVideoDriver = false;

    ALOGV("set Video Session [%d] state:%d", sessionId, state);
    // Check video session
    CHECK_VIDEO_SESSION_ID(sessionId, UNKNOWN_ERROR);
    if (mVideos[sessionId].getState() == state) {
        ALOGW("same video playback state %d for session %d", state, sessionId);
        return NO_ERROR;
    }

    if (mVideos[sessionId].setState(state) != NO_ERROR) {
        ALOGW("failed to update state %d for session %d", state, sessionId);
        return UNKNOWN_ERROR;
    }

    // Reset video session if player is closed
    if (state >= MDS_VIDEO_UNPREPARED) {
        mVideos[sessionId].init();
        ignoreVideoDriver = true;
    }

    int mode = mMode;
    if (hasVideoPlaying_l())
        mMode |= MDS_VIDEO_ON;
    else
        mMode &= ~MDS_VIDEO_ON;

    if (mMDSCallback != NULL)
        result = mMDSCallback->updateVideoState(sessionId, state);
    if (mode != mMode) {
        broadcastMessageLocked((int)MDS_MSG_MODE_CHANGE,
                &mMode, sizeof(mMode), ignoreVideoDriver);
    }

    return result;
}

MDS_VIDEO_STATE MultiDisplayComposer::getVideoState(int sessionId) {
    Mutex::Autolock lock(mMutex);
    // Check video session
    CHECK_VIDEO_SESSION_ID(sessionId, MDS_VIDEO_STATE_UNKNOWN);
    ALOGV("get Video Session [%d] state %d", sessionId, mVideos[sessionId].getState());
    return mVideos[sessionId].getState();
}

int MultiDisplayComposer::getVideoSessionNumber() {
    //TODO: avoid deadlock issue in HWC
    //Mutex::Autolock lock(mMutex);
    return getVideoSessionSize_l();
}

status_t MultiDisplayComposer::updateVideoSourceInfo(int sessionId, const MDSVideoSourceInfo& info) {
    MDC_CHECK_INIT();
    Mutex::Autolock lock(mMutex);
    ALOGV("mode[0x%x]protected[%d]w[%d]h[%d]fps[%d]interlace[%d]",
        mMode, info.isProtected, info.displayW,
        info.displayH, info.frameRate, info.isInterlaced);

    // Check video session
    CHECK_VIDEO_SESSION_ID(sessionId, UNKNOWN_ERROR);
    if (mVideos[sessionId].setInfo(info) != NO_ERROR)
        return UNKNOWN_ERROR;
    dumpVideoSession_l();
    return NO_ERROR;
}

status_t MultiDisplayComposer::getVideoSourceInfo(int sessionId, MDSVideoSourceInfo *info) {
    if (info == NULL)
        return BAD_VALUE;
    Mutex::Autolock lock(mMutex);
    // Check video session
    CHECK_VIDEO_SESSION_ID(sessionId, UNKNOWN_ERROR);
    if (mVideos[sessionId].getState() != MDS_VIDEO_PREPARED)
        return UNKNOWN_ERROR;
    return mVideos[sessionId].getInfo(info);
}

status_t MultiDisplayComposer::updatePhoneCallState(bool blank) {
    Mutex::Autolock lock(mMutex);
    ALOGV("the phone call state : %d", blank);
    if (mMDSCallback == NULL)
        return NO_INIT;
    return mMDSCallback->blankSecondaryDisplay(blank);
}

status_t MultiDisplayComposer::updateInputState(bool state) {
    Mutex::Autolock lock(mMutex);
    ALOGV("the input state:%d", state);
    if (mMDSCallback == NULL)
        return NO_INIT;
    return mMDSCallback->updateInputState(state);
}

status_t MultiDisplayComposer::setHdmiTiming(const MDSHdmiTiming& timing) {
    Mutex::Autolock lock(mMutex);

    if (mMDSCallback == NULL)
        return NO_INIT;
    MDSHdmiTiming real;
    memcpy(&real, &timing, sizeof(MDSHdmiTiming));
    if (!drm_hdmi_checkTiming(&real))
        return UNKNOWN_ERROR;

    return mMDSCallback->setHdmiTiming(real);
}

int MultiDisplayComposer::getHdmiTimingCount() {
    Mutex::Autolock lock(mMutex);

    return drm_hdmi_getTimingNumber();
}

status_t MultiDisplayComposer::getHdmiTimingList(
        int count, MDSHdmiTiming **list) {
    Mutex::Autolock lock(mMutex);
    bool ret = drm_hdmi_getTimings(count, list);
    return (ret == false ? UNKNOWN_ERROR : NO_ERROR);
}

status_t MultiDisplayComposer::getCurrentHdmiTiming(MDSHdmiTiming* timing) {
    Mutex::Autolock lock(mMutex);
    if (timing == NULL)
        return UNKNOWN_ERROR;
    return drm_hdmi_get_current_timing(timing);
}

status_t MultiDisplayComposer::setHdmiTimingByIndex(int index) {
    Mutex::Autolock lock(mMutex);

    return UNKNOWN_ERROR;
}

int MultiDisplayComposer::getCurrentHdmiTimingIndex() {
    Mutex::Autolock lock(mMutex);
    return -1;
}

status_t MultiDisplayComposer::setHdmiScalingType(MDS_SCALING_TYPE type) {
    ALOGV("set scaling type:%d", type);
    Mutex::Autolock lock(mMutex);
    status_t result = UNKNOWN_ERROR;
    // Check the callback implementation
    if (mMDSCallback != NULL)
        result = mMDSCallback->setHdmiScalingType(type);

    // If not implemented in callback, call SurfaceFlinger directly!
    if (result != NO_ERROR)
        result = setDisplayScalingLocked((uint32_t)type,
            mHorizontalStep, mVerticalStep);

    if (result == NO_ERROR)
        mScaleType = type;

    return result;
}

status_t MultiDisplayComposer::setHdmiOverscan(int hVal, int vVal) {
    Mutex::Autolock lock(mMutex);
    status_t result = UNKNOWN_ERROR;
    hVal = (hVal > overscan_max) ? 0: (overscan_max - hVal);
    vVal = (vVal > overscan_max) ? 0: (overscan_max - vVal);
    ALOGV("set overscan, h_val:%d, v_val:%d", hVal, vVal);
    // Check the callback implementation
    if (mMDSCallback != NULL)
        result = mMDSCallback->setHdmiOverscan(hVal, vVal);

    // If not implemented in callback, call SurfaceFlinger directly!
    if (result != NO_ERROR)
        result = setDisplayScalingLocked(
                (uint32_t)mScaleType, hVal, vVal);

    if (result == NO_ERROR) {
        mHorizontalStep = hVal;
        mVerticalStep = vVal;
    }
    return result;
}

MDS_DISPLAY_MODE MultiDisplayComposer::getDisplayMode(bool wait) {
    if (wait) {
        mMutex.lock();
    } else {
        if (mMutex.tryLock() == -EBUSY) {
            return MDS_MODE_NONE;
        }
    }
    //ALOGV("Mode is 0x%x, %d", mMode, wait);
    MDS_DISPLAY_MODE mode = (MDS_DISPLAY_MODE)mMode;
    mMutex.unlock();
    return mode;
}

int32_t MultiDisplayComposer::registerListener(
        const sp<IMultiDisplayListener>& listener,
        const char* name, int msg) {
    if (name == NULL || msg == 0 ||
            listener.get() == NULL) {
        ALOGE("Fail to register a new listener");
        return -1;
    }
    Mutex::Autolock _l(mMutex);
    if (mListeners.size() >= MDS_LISTENER_MAX_VALUE ||
            mListenerId >= MDS_LISTENER_MAX_VALUE) {
        ALOGE("Up to the maximum of listener %d", MDS_LISTENER_MAX_VALUE);
        return -1;
    }
    int32_t newId = mListenerId;
    for (size_t i = 0; i < mListeners.size(); i++) {
        if (mListeners.keyAt(i) == newId) {
            ALOGE("The listener %p is already registered!", listener.get());
            return -1;
        }
    }
    MultiDisplayListener* plistener =
        new MultiDisplayListener(msg, newId, name, listener);
    plistener->dump();
    mListeners.add(newId, plistener);
    mListenerId++;
    // Find a valid Id
    if (mListenerId >= MDS_LISTENER_MAX_VALUE) {
        mListenerId = 0;
        for (; mListenerId < MDS_LISTENER_MAX_VALUE; mListenerId++) {
            bool used = false;
            for (size_t i = 0; i < mListeners.size(); i++) {
                if (mListeners.keyAt(i) == mListenerId) {
                    used = true;
                    break;
                }
            }
            if (!used) break;
        }
        ALOGV("The next valid listener Id: %d", mListenerId);
    }
    return newId;
}

status_t MultiDisplayComposer::unregisterListener(int32_t listenerId) {
    Mutex::Autolock _l(mMutex);
    if (listenerId < 0) {
        ALOGE("Error listener ID");
        return BAD_VALUE;
    }
    for (size_t i = 0; i < mListeners.size(); i++) {
        if (mListeners.keyAt(i) != listenerId)
            continue;
        MultiDisplayListener* listener = mListeners.valueAt(i);
        if (listener == NULL)
            continue;
        ALOGV("Find a matched listener to unregister:\n");
        listener->dump();
        mListeners.removeItem(listenerId);
        delete listener;
        listener = NULL;
        break;
    }
    return NO_ERROR;
}

void MultiDisplayComposer::broadcastMessageLocked(
        int msg, void* value, int size, bool ignoreVideoDriver) {
    if (mListeners.size() == 0)
        return;

    for (size_t index = 0; index < mListeners.size(); index++) {
        MultiDisplayListener* listener = mListeners.valueAt(index);
        if (listener == NULL)
            continue;
        listener->dump();
        const char* name = listener->getName();
        if (ignoreVideoDriver && name != NULL &&
                !strncmp("VideoDriver", name, sizeof("VideoDriver"))) {
            ALOGV("Ignoring an invalid video driver message");
            continue;
        }
        if (listener->checkMsg(msg)) {
            sp<IMultiDisplayListener> ielistener = listener->getListener();
            if (ielistener != NULL)
                ielistener->onMdsMessage(msg, value, size);
        }
    }
}

status_t MultiDisplayComposer::setDisplayScalingLocked(uint32_t mode,
         uint32_t stepx, uint32_t stepy) {
    if (mSurfaceComposer == NULL) {
        const sp<IServiceManager> sm = defaultServiceManager();
        const String16 name("SurfaceFlinger");
        mSurfaceComposer = sm->getService(name);
        if (mSurfaceComposer == NULL) {
            return UNKNOWN_ERROR;
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

int MultiDisplayComposer::getVideoSessionSize_l() {
    int size = 0;
    for (int i = 0; i < MDS_VIDEO_SESSION_MAX_VALUE; i++) {
        if (mVideos[i].getState() != MDS_VIDEO_UNPREPARED)
            size++;
    }
    ALOGV("get video session number %d", size);
    return size;
}

int MultiDisplayComposer::allocateVideoSessionId() {
    Mutex::Autolock lock(mMutex);
    for (int i = 0; i < MDS_VIDEO_SESSION_MAX_VALUE; i++) {
        if (mVideos[i].getState() == MDS_VIDEO_UNPREPARED) {
            ALOGV("Allocate a new Video Session ID %d", i);
            return i;
        }
    }

    ALOGE("Fail to allocate session ID");
    return -1;
}

void MultiDisplayComposer::initVideoSessions_l() {
    for (int i = 0; i < MDS_VIDEO_SESSION_MAX_VALUE; i++) {
        mVideos[i].init();
    }
}

status_t MultiDisplayComposer::resetVideoPlayback() {
    Mutex::Autolock lock(mMutex);
    if (getVideoSessionSize_l() <= 0)
        return NO_ERROR;

    // TODO: for each video session, send MDS_VIDEO_UNPREPARED
    initVideoSessions_l();

    if (mMDSCallback != NULL) {
        mMDSCallback->updateVideoState(-1, MDS_VIDEO_UNPREPARED);
    }

    // exit extended mode
    mMode &= ~MDS_VIDEO_ON;
    broadcastMessageLocked((int)MDS_MSG_MODE_CHANGE, &mMode, sizeof(mMode), false);

    return NO_ERROR;
}

bool MultiDisplayComposer::hasVideoPlaying_l() {
    int size = 0;
    for (int i = 0; i < MDS_VIDEO_SESSION_MAX_VALUE; i++) {
        if (mVideos[i].getState() == MDS_VIDEO_PREPARED) {
            size++;
        }
    }
    return (size >= 1 ? true : false);
}

void MultiDisplayComposer::dumpVideoSession_l() {
    ALOGV("All Video Session info:\n");
    for (int i = 0; i < MDS_VIDEO_SESSION_MAX_VALUE; i++)
        mVideos[i].dump(i);
    return;
}

int MultiDisplayComposer::getValidDecoderConfigVideoSession_l() {
    int index = -1;
    int32_t width  = 0;
    int32_t height = 0;
    int32_t offX   = 0;
    int32_t offY   = 0;
    int32_t bufW   = 0;
    int32_t bufH   = 0;
    for (int i = 0; i < MDS_VIDEO_SESSION_MAX_VALUE; i++) {
        if (mVideos[i].getDecoderOutputResolution(
                    &width, &height, &offX, &offY, &bufW, &bufH) == NO_ERROR) {
            index = i;
            break;
        }
    }
    return index;
}

//TODO: The input "sessionId" is ignored now
status_t MultiDisplayComposer::getDecoderOutputResolution(
        int sessionId, int32_t* width, int32_t* height,
        int32_t* offX, int32_t* offY,
        int32_t* bufWidth, int32_t* bufHeight) {
    Mutex::Autolock lock(mMutex);
    status_t result = NO_ERROR;
    int index = getValidDecoderConfigVideoSession_l();
    if (index < 0)
        return UNKNOWN_ERROR;
    // Check video session
    CHECK_VIDEO_SESSION_ID(index, UNKNOWN_ERROR);
    result = mVideos[index].getDecoderOutputResolution(
            width, height, offX, offY, bufWidth, bufHeight);
    ALOGV("Video Session[%d]:Output resolution %dx%d, %dx%d, %dx%d",
            index, *width, *height, *offX, *offY, *bufWidth, *bufHeight);
    return result;
}

status_t MultiDisplayComposer::setDecoderOutputResolution(
        int sessionId, int32_t width, int32_t height,
        int32_t offX, int32_t offY,
        int32_t bufWidth, int32_t bufHeight) {
    Mutex::Autolock lock(mMutex);

    // Check video session
    CHECK_VIDEO_SESSION_ID(sessionId, UNKNOWN_ERROR);
    ALOGV("set video session %d decoder Output resolution %dx%d, %dx%d, %dx%d",
            sessionId, width, height, offX, offY, bufWidth, bufHeight);
    return mVideos[sessionId].setDecoderOutputResolution(
            width, height, offX, offY, bufWidth, bufHeight);
}

#ifdef TARGET_HAS_ISV
status_t MultiDisplayComposer::setVppState_l(
        MDS_DISPLAY_ID dpyId, bool connected, int status) {
    // TODO: only for WIDI now
    // the default Vpp policy for different display devices
    // HDMI/MIPI: is enabled
    // WIDI:      is disabled
    // the logic in here is tricky, and will revert it after
    // the usage of "MDS_WIDI_ON" is confirmed
    // only set widi connector status when status set to -1
    if (status == -1) {
        if (dpyId != MDS_DISPLAY_VIRTUAL) {
            return UNKNOWN_ERROR;
        }
        if (connected) {
            mDisplayId = dpyId;
        } else {
            mDisplayId = MDS_DISPLAY_PRIMARY;
        }
        ALOGV("Leaving %s, %d", __func__, mDisplayId);
    } else {
        ALOGI("%s: VPP setting changed, ready to broadcast message.", __func__);
        mMode |= MDS_VPP_CHANGED;
        broadcastMessageLocked((int)MDS_MSG_MODE_CHANGE, &mMode, sizeof(mMode), false);
        mMode &= ~MDS_VPP_CHANGED;
    }
    return NO_ERROR;
}

uint32_t MultiDisplayComposer::getVppState() {
    bool ret = false;
    uint32_t vpp_status = 0;

    Mutex::Autolock lock(mMutex);
    //TODO: only for WIDI now
    if (mDisplayId != MDS_DISPLAY_VIRTUAL)
        ret = VPPSetting::isVppOn(&vpp_status);
    ALOGV("%s: vpp on %d, display id %d, vpp_status %d", __func__, ret, mDisplayId, vpp_status);
    return vpp_status;
}

status_t MultiDisplayComposer::setVppState(
        MDS_DISPLAY_ID dpyId, bool connected, int status) {
    Mutex::Autolock lock(mMutex);
    ALOGV("%s:%d, %d, %d", __func__, __LINE__, dpyId, connected);
    return setVppState_l(dpyId, connected, status);
}
#endif

bool MultiDisplayComposer::checkHdmiTimingIsFixed() {
    Mutex::Autolock lock(mMutex);
    return drm_hdmi_timing_is_fixed();
}

}; // namespace intel
}; // namespace android
