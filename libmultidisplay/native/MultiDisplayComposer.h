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

#ifndef __MULTIDISPLAY_COMPOSER_H__
#define __MULTIDISPLAY_COMPOSER_H__

#include <utils/String16.h>
#include <utils/RefBase.h>
#include <utils/Vector.h>
#include <display/IMultiDisplayListener.h>
#include <display/IMultiDisplayCallback.h>
#include <display/IMultiDisplayInfoProvider.h>
#include <display/MultiDisplayType.h>

namespace android {
namespace intel {

static const int overscan_max = 5;

//Align with SF change
enum {
    SFIntelHDMIScalingSetting = 2001,
    SFIntelQueryPresentationMode,
    SFIntelPauseExternalDisplay
};

class MultiDisplayListener {
private:
    int      mMsg;
    int32_t  mId;
    String8* mName;
    sp<IMultiDisplayListener> mListener;
public:
    MultiDisplayListener(int msg, int32_t id,
            const char* client, sp<IMultiDisplayListener>);
    ~MultiDisplayListener();
    inline const char* getName() {
        if (mName != NULL)
            return mName->string();
        return NULL;
    }
    inline int getMsg() {
        return mMsg;
    }
    inline sp<IMultiDisplayListener> getListener() {
        return mListener;
    }
    inline bool checkMsg(int msg) {
        return (msg & mMsg) ? true : false;
    }
    inline int32_t getId() {
        return mId;
    }
    void dump();
};

class MultiDisplayVideoSession {
private:
    MDS_VIDEO_STATE     mState;
    MDSVideoSourceInfo  mInfo;
    bool                mInfoValid;
    // Decoder output
    int32_t             mDecoderConfigWidth;
    int32_t             mDecoderConfigHeight;
    int32_t             mDecoderConfigOffX;
    int32_t             mDecoderConfigOffY;
    int32_t             mDecoderConfigBufWidth;;
    int32_t             mDecoderConfigBufHeight;
    // TODO: Only 1 decoder config is valid now
    // May need support multiple decoder configs
    bool                mDecoderConfigValid;
public:

    inline MDS_VIDEO_STATE getState() {
        return mState;
    }
    inline status_t setState(MDS_VIDEO_STATE state) {
        if (state < MDS_VIDEO_PREPARING || state > MDS_VIDEO_UNPREPARED)
            return UNKNOWN_ERROR;
        mState = state;
        return NO_ERROR;
    }
    inline status_t getInfo(MDSVideoSourceInfo* info) {
        if (info == NULL || !mInfoValid)
            return UNKNOWN_ERROR;
        memcpy(info, &mInfo, sizeof(MDSVideoSourceInfo));
        return NO_ERROR;
    }
    inline status_t setInfo(const MDSVideoSourceInfo& info) {
        memcpy(&mInfo, &info, sizeof(MDSVideoSourceInfo));
        mInfoValid = true;
        return NO_ERROR;
    }
    inline status_t setDecoderOutputResolution(
            int32_t width, int32_t height,
            int32_t offX, int32_t offY,
            int32_t bufW, int32_t bufH) {
        if (mState < MDS_VIDEO_PREPARING || mState > MDS_VIDEO_UNPREPARED)
            return UNKNOWN_ERROR;
        mDecoderConfigValid  = true;
        mDecoderConfigWidth  = width;
        mDecoderConfigHeight = height;
        mDecoderConfigOffX = offX;
        mDecoderConfigOffY = offY;
        mDecoderConfigBufWidth   = bufW;
        mDecoderConfigBufHeight  = bufH;
        return NO_ERROR;
    }
    inline status_t getDecoderOutputResolution(
            int32_t* width, int32_t* height,
            int32_t* offX, int32_t* offY,
            int32_t* bufW, int32_t* bufH) {
        if (mState < MDS_VIDEO_PREPARING || mState > MDS_VIDEO_UNPREPARED)
            return UNKNOWN_ERROR;
        if (width == NULL || height == NULL ||
                offX == NULL || offY == NULL ||
                !mDecoderConfigValid)
            return UNKNOWN_ERROR;
        *width  = mDecoderConfigWidth;
        *height = mDecoderConfigHeight;
        *offX   = mDecoderConfigOffX;
        *offY   = mDecoderConfigOffY;
        *bufW   = mDecoderConfigBufWidth;
        *bufH   = mDecoderConfigBufHeight;
        return NO_ERROR;
    }
    inline void init() {
        mState = MDS_VIDEO_UNPREPARED;
        memset(&mInfo, 0, sizeof(MDSVideoSourceInfo));
        mInfoValid = false;
        mDecoderConfigValid  = false;
    }
    void dump(int index);
};

class MultiDisplayComposer : public RefBase {
public:
    MultiDisplayComposer();
    virtual ~MultiDisplayComposer();

    // Video control
    int allocateVideoSessionId();
    status_t updateVideoState(int, MDS_VIDEO_STATE);
    status_t resetVideoPlayback();
    status_t updateVideoSourceInfo(int, const MDSVideoSourceInfo&);

    // Infomation provider
    int getVideoSessionNumber();
    MDS_VIDEO_STATE getVideoState(int);
    status_t getVideoSourceInfo(int, MDSVideoSourceInfo*);
    MDS_DISPLAY_MODE getDisplayMode(bool);
    uint32_t getVppState();

    // Sink Registrar
    int32_t  registerListener(const sp<IMultiDisplayListener>&, const char*, int);
    status_t unregisterListener(int32_t listenerId);

    // Callback registrar
    status_t registerCallback(const sp<IMultiDisplayCallback>&);
    status_t unregisterCallback(const sp<IMultiDisplayCallback>&);

    // Hdmi control
    status_t setHdmiTiming(const MDSHdmiTiming&);
    int getHdmiTimingCount();
    status_t getHdmiTimingList(int, MDSHdmiTiming**);
    status_t getCurrentHdmiTiming(MDSHdmiTiming*);
    status_t setHdmiTimingByIndex(int);
    int getCurrentHdmiTimingIndex();
    status_t setHdmiScalingType(MDS_SCALING_TYPE);
    status_t setHdmiOverscan(int, int);
    bool checkHdmiTimingIsFixed();

    // Display connection state observer
    status_t updateHdmiConnectionStatus(bool);
    status_t updateWidiConnectionStatus(bool);

    // Event monitor
    status_t updateInputState(bool);
    status_t updatePhoneCallState(bool);

    // Decoder configure
    status_t getDecoderOutputResolution(int, int32_t*, int32_t*, int32_t*, int32_t*, int32_t*, int32_t*);
    status_t setDecoderOutputResolution(int, int32_t,  int32_t, int32_t, int32_t, int32_t, int32_t);

#ifdef TARGET_HAS_ISV
    // Vpp configure
    status_t setVppState(MDS_DISPLAY_ID, bool, int);
#endif

private:
    // Assume it is impossible that there are up to 64 cocurrent running video driver
    static const int MDS_LISTENER_MAX_VALUE = (MDS_VIDEO_SESSION_MAX_VALUE * 4);
    bool     mDrmInit;
    int      mMode;
    mutable  Mutex mMutex;
    uint32_t mHorizontalStep;
    uint32_t mVerticalStep;
#ifdef TARGET_HAS_ISV
    // Vpp cofigure, only for WIDI now, and will be removed
    // Maintain current connected display device Id
    // if HDMI is connected, this id = MDS_DISPLAY_EXTERNAL
    // if WIFI is connected, this id = MDS_DISPLAY_VIRTUAL
    // else id = MDS_DISPLAY_PRIMARY
    MDS_DISPLAY_ID mDisplayId;
#endif
    MDS_SCALING_TYPE mScaleType;
    int32_t mListenerId;

    sp<IBinder> mSurfaceComposer;
    sp<IMultiDisplayCallback> mMDSCallback;

    KeyedVector<int32_t, MultiDisplayListener* > mListeners;
    MultiDisplayVideoSession mVideos[MDS_VIDEO_SESSION_MAX_VALUE];

    void init();
    void broadcastMessageLocked(int msg, void* value, int size, bool ignoreVideoDriver);
    status_t setDisplayScalingLocked(uint32_t mode, uint32_t stepx, uint32_t stepy);
    status_t updateHdmiConnectStatusLocked();
    MultiDisplayVideoSession* getVideoSession_l(int sessionId);
    int  getVideoSessionSize_l();
    void initVideoSessions_l();
    bool hasVideoPlaying_l();
    void dumpVideoSession_l();
    int  getValidDecoderConfigVideoSession_l();
    status_t notifyHotplugLocked(MDS_DISPLAY_ID, bool);
#ifdef TARGET_HAS_ISV
    status_t setVppState_l(MDS_DISPLAY_ID, bool, int);
#endif

    inline bool checkMode(int value, int bit) {
        return (value & bit) == bit ? true : false;
    }
};

}; // namespace intel
}; // namespace android

#endif
