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

#ifndef __ANDROID_INTEL_IMULTIDISPLAY_EVENTMONITOR_H__
#define __ANDROID_INTEL_IMULTIDISPLAY_EVENTMONITOR_H__

#include <utils/Errors.h>
#include <utils/RefBase.h>
#include <binder/IInterface.h>

#include <display/MultiDisplayType.h>

namespace android {
namespace intel {


class IMultiDisplayEventMonitor : public IInterface {
public:
    DECLARE_META_INTERFACE(MultiDisplayEventMonitor);
    /**
     * @brief Update the state of phone call
     * @param bool state, true indicates there is a call is handling
     * @return @see status_t in <utils/Errors.h>
     */
    virtual status_t updatePhoneCallState(bool state) = 0;
    /**
     * @brief Update the state of touch screen and key input
     * @param bool state, true indicates there is input event
     * @return @see status_t in <utils/Errors.h>
     */
    virtual status_t updateInputState(bool state) = 0;
};

class BnMultiDisplayEventMonitor : public BnInterface<IMultiDisplayEventMonitor> {
public:
    virtual status_t onTransact(uint32_t code,
                                const Parcel& data,
                                Parcel* replay,
                                uint32_t flags = 0);
};

}; // namespace intel
}; // namespace android

#endif
