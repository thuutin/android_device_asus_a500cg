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

#ifndef __ANDROID_INTEL_IMULTIDISPLAY_SINKREGISTRAR_H__
#define __ANDROID_INTEL_IMULTIDISPLAY_SINKREGISTRAR_H__

#include <utils/Errors.h>
#include <utils/RefBase.h>
#include <binder/IInterface.h>

#include <display/MultiDisplayType.h>
#include <display/IMultiDisplayListener.h>

namespace android {
namespace intel {


class IMultiDisplaySinkRegistrar : public IInterface {
public:
    DECLARE_META_INTERFACE(MultiDisplaySinkRegistrar);
    /**
     * @brief Register display listener, those modules which need to get
     * info from MDS can register a listener to get info, avoid to use
     * polling method.
     * @param listener: inherit and implement IMultiDisplayListener.
     * @param name: client name, ensure it is not a null pointer
     * @param msg: messge type, @see MDS_MESSAGE in MultiDisplayType.h
     * @return: return a listener Id
     *          >= 0 indicate a valid listenner
     *          <  0 indicate an invalid listener
     */
    virtual int32_t registerListener(
            const sp<IMultiDisplayListener>&, const char* name, int msg) = 0;

    /**
     * @brief Unregister the display listener
     * @param
     * @return @see status_t in <utils/Errors.h>
     */
    virtual status_t unregisterListener(int32_t listenerId) = 0;
};


class BnMultiDisplaySinkRegistrar : public BnInterface<IMultiDisplaySinkRegistrar> {
public:
    virtual status_t onTransact(uint32_t code,
                                const Parcel& data,
                                Parcel* replay,
                                uint32_t flags = 0);
};

}; // namespace intel
}; // namespace android

#endif
