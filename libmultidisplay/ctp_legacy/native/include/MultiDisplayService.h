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

#ifndef ANDROID_MULTIDISPLAYSERVICE_H
#define ANDROID_MULTIDISPLAYSERVICE_H
#include <utils/RefBase.h>
#include <binder/IInterface.h>
#include <binder/Parcel.h>
#include <binder/BinderService.h>
#include <display/IMultiDisplayComposer.h>
#include <display/MultiDisplayComposer.h>
#include <display/IExtendDisplayListener.h>

namespace android {
namespace intel {

#define INTEL_MDS_SERVICE_NAME "display.intel.mds"

class MultiDisplayService:
    public BinderService<MultiDisplayService>,
    public BnMultiDisplayComposer {
private:
    MultiDisplayComposer* mMDC;
public:
    static char* const getServiceName() { return INTEL_MDS_SERVICE_NAME; }
    static void instantiate();

    MultiDisplayService();
    ~MultiDisplayService();

    int getMode(bool wait);
    int notifyWidi(bool);
    int notifyMipi(bool);
    int setModePolicy(int);
    int notifyHotPlug();
    int setHdmiPowerOff();
    int prepareForVideo(int);
    int getVideoState();
    int updateVideoInfo(const MDSVideoSourceInfo&);

    int registerListener(sp<IExtendDisplayListener>, void *, const char *, int);
    int unregisterListener(void *);

    int getHdmiModeInfo(int* widht, int* height, int* refresh, int* interlace, int* ratio);
    int setHdmiModeInfo(int widht, int height, int refresh, int interlace, int ratio);
    int setHdmiScaleType(int type);
    int setHdmiScaleStep(int hValue, int vValue);
    int getHdmiDeviceChange();
    int getVideoInfo(int* dw, int* dh, int* fps, int* interlace);
    int getDisplayCapability();
};

}; // namespace intel
}; // namespace android

#endif
