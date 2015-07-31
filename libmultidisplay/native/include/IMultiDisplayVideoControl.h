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

#ifndef __ANDROID_ITNTEL_IMULTIDISPLAYVIDEOCONTROL_H__
#define __ANDROID_ITNTEL_IMULTIDISPLAYVIDEOCONTROL_H__

#include <utils/Errors.h>
#include <utils/RefBase.h>
#include <binder/IInterface.h>

#include <display/MultiDisplayType.h>

namespace android {
namespace intel {


class IMultiDisplayVideoControl : public IInterface {
public:
    DECLARE_META_INTERFACE(MultiDisplayVideoControl);
    /**
     * @brief Allocate a unique id for video player
     * @return: a sessionId
     */
    virtual int allocateVideoSessionId() = 0;

    /**
     * @brief Reset video playback
     * @return @see status_t in <utils/Errors.h>
     */
    virtual status_t resetVideoPlayback() = 0;

    /**
     * @brief Set the state of video playback
     * @param state @see MDS_VIDEO_STATE in MultiDisplayType.h
     * @return @see status_t in <utils/Errors.h>
     */
    virtual status_t updateVideoState(int sessionId, MDS_VIDEO_STATE state) = 0;

    /**
     * @brief Update the video playback info
     * @param sessionId, a unique id for every video player
     * @param info @see VideoSourceInfo in the MultiDisplayType.h
     * @return @see status_t in <utils/Errors.h>
     */
    virtual status_t updateVideoSourceInfo(int sessionId, const MDSVideoSourceInfo& info) = 0;
};

class BnMultiDisplayVideoControl : public BnInterface<IMultiDisplayVideoControl> {
public:
    virtual status_t onTransact(uint32_t code,
                                const Parcel& data,
                                Parcel* replay,
                                uint32_t flags = 0);
};


}; // namespace intel
}; // namespace android

#endif
