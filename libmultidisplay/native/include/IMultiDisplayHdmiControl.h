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

#ifndef __ANDROID_INTEL_IMULTIDISPLAYHDMICONTROL_H__
#define __ANDROID_INTEL_IMULTIDISPLAYHDMICONTROL_H__

#include <utils/Errors.h>
#include <utils/RefBase.h>
#include <binder/IInterface.h>

#include <display/MultiDisplayType.h>

namespace android {
namespace intel {


class IMultiDisplayHdmiControl : public IInterface {
public:
    DECLARE_META_INTERFACE(MultiDisplayHdmiControl);
    /**
     * @brief Set the Hdmi timing according to the input timing parameter
     * @param timing The timing which is going to be set. @see MDSHdmiTiming
     * @return @see status_t in <utils/Errors.h>
     */
    virtual status_t setHdmiTiming(const MDSHdmiTiming& timing) = 0;

    /**
     * @brief Get the timing count of HDMI
     * @return The total timing count supported by display device
     */
    virtual int getHdmiTimingCount() = 0;

    /**
     * @brief Get the timing list of HDMI
     * @param count the total timing count, must equal to the value from \
     *        @see getHdmiTimingCount
     * @param list  the timing list
     * return: @see status_t in <utils/Errors.h>
     */
    virtual status_t getHdmiTimingList(int count, MDSHdmiTiming** list) = 0;

    /**
     * @brief Get the HDMI timing which is used now
     * @param timing The current timing in use
     * @return: @see status_t in <utils/Errors.h>
     */
    virtual status_t getCurrentHdmiTiming(MDSHdmiTiming* timing) = 0;

    /**
     * @brief Set the timing of hdmi according to the input timing index
     * @param index The index in the timing list.
     * @return: @see status_t in <utils/Errors.h>
     */
    virtual status_t setHdmiTimingByIndex(int index) = 0;

    /**
     * @brief Get the timing index which is used now, only for HDMI
     * @return current timing index in the list
     */
    virtual int getCurrentHdmiTimingIndex() = 0;

    /**
     * @brief Set the scale type of HDMI
     * @param type  @see MDS_SCALING_TYPE in MultiDisplayType.h
     * @return @see status_t in <utils/Errors.h>
     */
    virtual status_t setHdmiScalingType(MDS_SCALING_TYPE type) = 0;

    /**
     * @brief Set the overscan compensation for HDMI
     * @param hValue
     * @param vValue
     * @return @see status_t in <utils/Errors.h>
     */
    virtual status_t setHdmiOverscan(int hValue, int vValue) = 0;

    /**
     * @brief check HDMI timing whether is fixed
     * @param
     * @return true: fixed, false: not fixed
     */
    virtual bool checkHdmiTimingIsFixed() = 0;
};

class BnMultiDisplayHdmiControl : public BnInterface<IMultiDisplayHdmiControl> {
public:
    virtual status_t onTransact(uint32_t code,
                                const Parcel& data,
                                Parcel* replay,
                                uint32_t flags = 0);
};

}; // namespace intel
}; // namespace android

#endif
