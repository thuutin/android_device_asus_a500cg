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

#ifndef __ANDROID_INTEL_MULTIDISPLAYSERVICE_H__
#define __ANDROID_INTEL_MULTIDISPLAYSERVICE_H__

#include <utils/RefBase.h>
#include <binder/IInterface.h>
#include <binder/Parcel.h>
#include <binder/BinderService.h>

#include <display/IMultiDisplayHdmiControl.h>
#include <display/IMultiDisplayVideoControl.h>
#include <display/IMultiDisplayEventMonitor.h>
#include <display/IMultiDisplayCallbackRegistrar.h>
#include <display/IMultiDisplaySinkRegistrar.h>
#include <display/IMultiDisplayInfoProvider.h>
#include <display/IMultiDisplayConnectionObserver.h>
#include <display/IMultiDisplayDecoderConfig.h>
#ifdef TARGET_HAS_ISV
#include <display/IMultiDisplayVppConfig.h>
#endif


namespace android {
namespace intel {

#define INTEL_MDS_SERVICE_NAME "display.intel.mds"

class IMDService : public IInterface {
public:
    DECLARE_META_INTERFACE(MDService);
    virtual sp<IMultiDisplayHdmiControl>         getHdmiControl() = 0;
    virtual sp<IMultiDisplayVideoControl>        getVideoControl() = 0;
    virtual sp<IMultiDisplayEventMonitor>        getEventMonitor()  = 0;
    virtual sp<IMultiDisplayCallbackRegistrar>   getCallbackRegistrar() = 0;
    virtual sp<IMultiDisplaySinkRegistrar>       getSinkRegistrar() = 0;
    virtual sp<IMultiDisplayInfoProvider>        getInfoProvider() = 0;
    virtual sp<IMultiDisplayConnectionObserver>  getConnectionObserver() = 0;
    virtual sp<IMultiDisplayDecoderConfig>       getDecoderConfig() = 0;
#ifdef TARGET_HAS_ISV
    virtual sp<IMultiDisplayVppConfig>           getVppConfig() = 0;
#endif
};

class BnMDService : public BnInterface<IMDService> {
public:
    virtual status_t onTransact(uint32_t code,
                                const Parcel& data,
                                Parcel* replay,
                                uint32_t flags = 0);
};

class MultiDisplayService : public BnMDService {
public:
    MultiDisplayService();
    ~MultiDisplayService();

    static char* const getServiceName() { return INTEL_MDS_SERVICE_NAME; }
    static void instantiate();

    virtual sp<IMultiDisplayHdmiControl>         getHdmiControl();
    virtual sp<IMultiDisplayVideoControl>        getVideoControl();
    virtual sp<IMultiDisplayEventMonitor>        getEventMonitor();
    virtual sp<IMultiDisplayCallbackRegistrar>   getCallbackRegistrar();
    virtual sp<IMultiDisplaySinkRegistrar>       getSinkRegistrar();
    virtual sp<IMultiDisplayInfoProvider>        getInfoProvider();
    virtual sp<IMultiDisplayConnectionObserver>  getConnectionObserver();
    virtual sp<IMultiDisplayDecoderConfig>       getDecoderConfig();
#ifdef TARGET_HAS_ISV
    virtual sp<IMultiDisplayVppConfig>           getVppConfig();
#endif
};

}; // namespace intel
}; // namespace android

#endif
