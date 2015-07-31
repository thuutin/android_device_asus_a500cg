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

#ifndef __IMULTIDISPLAY_CALLBACK_H__
#define __IMULTIDISPLAY_CALLBACK_H__

#include <utils/Errors.h>
#include <utils/RefBase.h>
#include <binder/IInterface.h>

#include <display/MultiDisplayType.h>

namespace android {
namespace intel {

class IMultiDisplayCallback : public IInterface {
public:
    DECLARE_META_INTERFACE(MultiDisplayCallback);

    /*
     * set the state of the video playback
     * param: please refer MultiDisplayType.h
     * return:
     *       0: on success
     *     !=0: on failure
     */
    virtual status_t updateVideoState(int SessionId, MDS_VIDEO_STATE state) = 0;

    /*
     * set the resolution of the display, only for HDMI
     * param: please refer MultiDisplayType.h
     * return:
     *       0: on success
     *     !=0: on failure
     */
    virtual status_t setHdmiTiming(const MDSHdmiTiming& timing) = 0;
    /*
     * blank the secondary display
     * param: please refer MultiDisplayType.h
     * return:
     *       0: on success
     *     !=0: on failure
     */
    virtual status_t blankSecondaryDisplay(bool blank) = 0;
    /*
     * set the scale type of display
     * param: please refer MultiDisplayType.h
     * return:
     *       0: on success
     *     !=0: on failure
     */
    virtual status_t setHdmiScalingType(MDS_SCALING_TYPE type) = 0;

    /*
     * set the overscan compensation of display
     * param: please refer MultiDisplayType.h
     * return:
     *       0: on success
     *     !=0: on failure
     */
    virtual status_t setHdmiOverscan(int hValue, int vValue) = 0;
    /*
     * set the state of touch screen or key input
     * param: please refer MultiDisplayType.h
     * return:
     *       0: on success
     *     !=0: on failure
     */
    virtual status_t updateInputState(bool state) = 0;
};

class BnMultiDisplayCallback : public BnInterface<IMultiDisplayCallback>
{
public:
    virtual status_t onTransact( uint32_t code,
                                 const Parcel& data,
                                 Parcel* reply,
                                 uint32_t flags = 0);

};

}; // namespace intel
}; // namespace android

#endif /* __IMULTIDISPLAY_CALLBACK_H__ */


