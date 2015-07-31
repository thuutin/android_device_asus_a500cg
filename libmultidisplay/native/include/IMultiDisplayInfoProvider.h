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

#ifndef __ANDROID_INTEL_IMULTIDISPLAY_INFOPROVIDER_H__
#define __ANDROID_INTEL_IMULTIDISPLAY_INFOPROVIDER_H__

#include <utils/Errors.h>
#include <utils/RefBase.h>
#include <binder/IInterface.h>

#include <display/MultiDisplayType.h>

namespace android {
namespace intel {


/**
 * @brief The display related mode info: \n
 * '1' means the bit is valid \n
 * '0' means the bit is invalid
 */
typedef enum {
    MDS_MODE_NONE       = 0,       /**< No bit is set */
    MDS_DVI_CONNECTED   = 1,       /**< DVI is connected */
    MDS_HDMI_CONNECTED  = 1 << 1,  /**< HDMI is connected */
    MDS_WIDI_ON         = 1 << 2,  /**< WIDI is connected*/
    MDS_VIDEO_ON        = 1 << 3,  /**< Video is playing */
    MDS_VPP_CHANGED     = 1 << 4,  /**< VPP status is changed */
} MDS_DISPLAY_MODE;

class IMultiDisplayInfoProvider : public IInterface {
public:
    DECLARE_META_INTERFACE(MultiDisplayInfoProvider);
    /**
     * @brief Get the state of video playback
     * @param
     * @return: the total number of video playback instance
     */
     virtual int getVideoSessionNumber() = 0;
    /**
     * @brief Get the state of video playback
     * @param sessionId, a unique id for every video player
     * @return @see MDS_VIDEO_STATE in MultiDisplayType.h
     */
     virtual MDS_VIDEO_STATE getVideoState(int sessionID) = 0;

    /**
     * @brief Get video source info
     * @param sessionId, a unique id for every video player
     * @param info @see VideoSourceInfo in the MultiDisplayType.h
     * @return @see status_t in <utils/Errors.h>
     */
    virtual status_t getVideoSourceInfo(int sessionId, MDSVideoSourceInfo* info) = 0;

    /**
     * @brife Get display mode
     * @param wait "ture" means this interface will be blocked
     *             until getting an accurate mode,
     *             "false" means it will return immediatly,
     *             but couldn't ensure the mode is right.
     * @return: @see MDS_DISPLAY_MODE
     */
    virtual MDS_DISPLAY_MODE getDisplayMode(bool wait) = 0;

    /**
     * @brief Get the decoder configure
     * @param
     *         int videoSessionId: Video Session id,
     *                             for support multiple decoder configure.
     *         int32_t* width:      the width  of decoder output
     *         int32_t* height:     the height of decoder output
     * @return @see status_t in <utils/Errors.h>
     */
     virtual status_t getDecoderOutputResolution(
             int videoSessionId,
             int32_t* width, int32_t* height,
             int32_t* offX, int32_t* offY,
             int32_t* bufW, int32_t* bufH) = 0;

    /**
     * @brief Get the vpp state for current display device, include HDMI, WIDI and MIPI
     * @param
     * @return @see VPP_SETTING_STATUS in VPPSetting.h
     */
     virtual uint32_t getVppState() = 0;
};


class BnMultiDisplayInfoProvider : public BnInterface<IMultiDisplayInfoProvider> {
public:
    virtual status_t onTransact(uint32_t code,
                                const Parcel& data,
                                Parcel* replay,
                                uint32_t flags = 0);
};

}; // namespace intel
}; // namespace android

#endif
