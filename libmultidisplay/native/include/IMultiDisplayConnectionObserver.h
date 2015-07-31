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

#ifndef __ANDROID_INTEL_IMULTIDISPLAY_CONNECTIONOBSERVER_H__
#define __ANDROID_INTEL_IMULTIDISPLAY_CONNECTIONOBSERVER_H__

#include <utils/Errors.h>
#include <utils/RefBase.h>
#include <binder/IInterface.h>

#include <display/MultiDisplayType.h>

namespace android {
namespace intel {


class IMultiDisplayConnectionObserver : public IInterface {
public:
    DECLARE_META_INTERFACE(MultiDisplayConnectionObserver);
    /**
     * @brief update HDMI hotplug event to MDS
     *
     * @param connected: "true" indicate receiving a HDMI connected event @
     * @return @see status_t in <utils/Errors.h>
     */
    virtual status_t updateHdmiConnectionStatus(bool connected) = 0;
    /**
     * @brief update widi video extended mode to MDS
     *
     * @param connected: "true" indicate WIDI video playback is working under extended mode @
     * @return @see status_t in <utils/Errors.h>
     */
    virtual status_t updateWidiConnectionStatus(bool connected) = 0;
};


class BnMultiDisplayConnectionObserver : public BnInterface<IMultiDisplayConnectionObserver> {
public:
    virtual status_t onTransact(uint32_t code,
                                const Parcel& data,
                                Parcel* replay,
                                uint32_t flags = 0);
};

}; // namespace intel
}; // namespace android

#endif
