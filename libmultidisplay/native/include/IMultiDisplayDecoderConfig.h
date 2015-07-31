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

#ifndef __ANDROID_INTEL_IMULTIDISPLAY_DECODERCONFIG_H__
#define __ANDROID_INTEL_IMULTIDISPLAY_DECODERCONFIG_H__

#include <utils/Errors.h>
#include <utils/RefBase.h>
#include <binder/IInterface.h>

#include <display/MultiDisplayType.h>

namespace android {
namespace intel {


class IMultiDisplayDecoderConfig : public IInterface {
public:
    DECLARE_META_INTERFACE(MultiDisplayDecoderConfig);
    /**
     * @brief Set the decoder output width and height
     * @param
     * @return @see status_t in <utils/Errors.h>
     */
    virtual status_t setDecoderOutputResolution(
            int32_t videoSessionId,
            int32_t width, int32_t height,
            int32_t offX,  int32_t offY,
            int32_t bufWidth, int32_t bufHeight) = 0;
};


class BnMultiDisplayDecoderConfig : public BnInterface<IMultiDisplayDecoderConfig> {
public:
    virtual status_t onTransact(uint32_t code,
                                const Parcel& data,
                                Parcel* replay,
                                uint32_t flags = 0);
};

}; // namespace intel
}; // namespace android

#endif
