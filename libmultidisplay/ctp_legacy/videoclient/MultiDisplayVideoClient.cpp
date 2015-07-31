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
 *
 * Adding this file to reduce framework/av hack code lines
 */

//#define LOG_NDEBUG 0

#include <utils/Log.h>
#include <utils/RefBase.h>
#include <binder/Parcel.h>

#include <binder/IServiceManager.h>
#include <binder/IPCThreadState.h>

#include <display/MultiDisplayVideoClient.h>


namespace android {
namespace intel {


MultiDisplayVideoClient::MultiDisplayVideoClient() {
    mSessionId = -1;
    mState = MDS_VIDEO_UNPREPARED;
    mVideo = NULL;
};


MultiDisplayVideoClient::~MultiDisplayVideoClient() {
    // ALOGI("Destroy mds video client %d", mSessionId);
    close();
};

void MultiDisplayVideoClient::close() {
    mSessionId = -1;
    mState = MDS_VIDEO_UNPREPARED;
    mVideo = NULL;
}

status_t MultiDisplayVideoClient::prepare(int state, bool isProtected) {
    if (state >= MDS_VIDEO_UNPREPARING && mSessionId < 0) {
        ALOGW("Invalid unprepared state");
        return UNKNOWN_ERROR;
    }
    if (state == mState) {
        // ALOGW("Same state %d", state);
        return UNKNOWN_ERROR;
    }
    if (!isProtected &&
            (state == MDS_VIDEO_PREPARING ||
             state == MDS_VIDEO_UNPREPARING)) {
        // ignore preparing and unpreparing state if video is not protected
        ALOGI("Ignore MDS preparing and unpreparing for clear content");
        return UNKNOWN_ERROR;
    }
    ALOGI("Video state is %d, %d", state, mState);
    if (mVideo == NULL) {
        mVideo = new MultiDisplayClient();
    }
    if (mSessionId < 0) {
        mSessionId = 0;//mVideo->allocateVideoSessionId();
    }
    return NO_ERROR;
}
status_t MultiDisplayVideoClient::setVideoState(int state,
        bool isProtected, const sp<MetaData> &meta) {
    if (prepare(state, isProtected) != NO_ERROR)
        return UNKNOWN_ERROR;

    if (state == MDS_VIDEO_PREPARED) {
        MDSVideoSourceInfo info;
        memset(&info, 0, sizeof(MDSVideoSourceInfo));
        info.isProtected = isProtected;
        info.isPlaying = true;
        if (meta != NULL && meta.get() != NULL) {
            if (!meta->findInt32(kKeyFrameRate, &info.frameRate)) {
                info.frameRate = 0;
            }
            if (!meta->findInt32(kKeyDisplayWidth, &info.displayW)) {
                info.displayW = 0;
            }
            if (!meta->findInt32(kKeyDisplayHeight, &info.displayH)) {
                info.displayH = 0;
            }
        }
        mVideo->updateVideoSourceInfo(mSessionId, info);
        ALOGI("%s: Video source Info %d x %d @ %d fps", __func__,
               info.displayW, info.displayH, info.frameRate);
    }
    mVideo->updateVideoState(mSessionId, (MDS_VIDEO_STATE)state);
    if (state == MDS_VIDEO_UNPREPARED)
        close();
    mState = state;
    return NO_ERROR;
}

status_t MultiDisplayVideoClient::setVideoState(int state,
        bool isProtected, const sp<AMessage> &msg) {
    if (prepare(state, isProtected) != NO_ERROR)
        return UNKNOWN_ERROR;
    if (state == MDS_VIDEO_PREPARED) {
        if (msg != NULL && msg.get() != NULL) {
            MDSVideoSourceInfo info;
            memset(&info, 0, sizeof(MDSVideoSourceInfo));
            info.isPlaying = true;
            info.isProtected = isProtected;
            bool success = msg->findInt32("frame-rate", &info.frameRate);
            if (!success)
                info.frameRate = 0;
            success = msg->findInt32("width", &info.displayW);
            if (!success)
                info.displayW = 0;
            success = msg->findInt32("height", &info.displayH);
            if (!success)
                info.displayH = 0;
            mVideo->updateVideoSourceInfo(mSessionId, info);
            ALOGI("%s: Video source Info %d x %d @ %d fps ", __func__,
                     info.displayW, info.displayH, info.frameRate);
        }
    }
    mVideo->updateVideoState(mSessionId, (MDS_VIDEO_STATE)state);
    if (state == MDS_VIDEO_UNPREPARED)
        close();
    mState = state;
    return NO_ERROR;
}

status_t MultiDisplayVideoClient::reset() {
    if (mVideo != NULL)
        return  mVideo->resetVideoPlayback();
    else
        return UNKNOWN_ERROR;
}

}; // namespace intel
}; // namespace android
