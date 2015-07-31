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

//#define LOG_NDEBUG 0
#include <utils/Log.h>
#include <binder/IServiceManager.h>
#include <binder/IPCThreadState.h>
#include <display/MultiDisplayType.h>
#include <display/MultiDisplayService.h>


namespace android {
namespace intel {




#define MDS_CHECK_MDS() \
do { \
    if (mMDC == NULL) { \
        ALOGE("%s: MDSComposer is null", __func__); \
        return MDS_ERROR; \
    } \
} while(0)

MultiDisplayService::MultiDisplayService() {
    ALOGV("%s: create a MultiDisplay service, %p", __func__, this);
    mMDC = new MultiDisplayComposer();
}

MultiDisplayService::~MultiDisplayService() {
    if (mMDC != NULL) {
        delete mMDC;
        mMDC = NULL;
    }
}

void MultiDisplayService::instantiate() {
    sp<IServiceManager> sm(defaultServiceManager());
    if (sm->addService(String16(INTEL_MDS_SERVICE_NAME), new MultiDisplayService()))
        ALOGE("Failed to start %s service", INTEL_MDS_SERVICE_NAME);
}

int MultiDisplayService::getMode(bool wait) {
    MDS_CHECK_MDS();
    return mMDC->getMode(wait);
}

int MultiDisplayService::notifyWidi(bool on) {
    MDS_CHECK_MDS();
    return mMDC->notifyWidi(on);
}

int MultiDisplayService::notifyMipi(bool on) {
    MDS_CHECK_MDS();
    return mMDC->notifyMipi(on);
}

int MultiDisplayService::prepareForVideo(int status) {
    MDS_CHECK_MDS();
    return mMDC->prepareForVideo(status);
}

int MultiDisplayService::getVideoState() {
    MDS_CHECK_MDS();
    return mMDC->getVideoState();
}

int MultiDisplayService::updateVideoInfo(const MDSVideoSourceInfo& info) {
    MDS_CHECK_MDS();
    return mMDC->updateVideoInfo(info);
}

int MultiDisplayService::notifyHotPlug() {
    MDS_CHECK_MDS();
    return mMDC->notifyHotPlug();
}

int MultiDisplayService::setHdmiPowerOff() {
    MDS_CHECK_MDS();
    return mMDC->setHdmiPowerOff();
}

int MultiDisplayService::setModePolicy(int policy) {
    MDS_CHECK_MDS();
    return mMDC->setModePolicy(policy);
}

int MultiDisplayService::registerListener(
                            sp<IExtendDisplayListener> listener,
                            void *handle, const char* client, int msg) {
    MDS_CHECK_MDS();
    return mMDC->registerListener(listener, handle, client, msg);
}

int MultiDisplayService::unregisterListener(void *handle) {
    MDS_CHECK_MDS();
    return mMDC->unregisterListener(handle);
}

int MultiDisplayService::getHdmiModeInfo(int* pWidth, int* pHeight,
                                         int* pRefresh, int* pInterlace, int* pRatio) {
    MDS_CHECK_MDS();
    return mMDC->getHdmiModeInfo(pWidth, pHeight, pRefresh, pInterlace, pRatio);
}

int MultiDisplayService::setHdmiModeInfo(int width, int height, int refresh, int interlace, int ratio) {
    MDS_CHECK_MDS();
    return mMDC->setHdmiModeInfo(width, height, refresh, interlace, ratio);
}
int MultiDisplayService::setHdmiScaleType(int type) {
    return mMDC->setHdmiScaleType(type);
}

int MultiDisplayService::setHdmiScaleStep(int hValue, int vValue) {
    MDS_CHECK_MDS();
    return mMDC->setHdmiScaleStep(hValue, vValue);
}

int MultiDisplayService::getHdmiDeviceChange() {
    MDS_CHECK_MDS();
    return mMDC->getHdmiDeviceChange();
}

int MultiDisplayService::getVideoInfo(int* dw, int* dh, int* fps, int* interlace) {
    MDS_CHECK_MDS();
    return mMDC->getVideoInfo(dw, dh, fps, interlace);
}

int MultiDisplayService::getDisplayCapability() {
    MDS_CHECK_MDS();
    return mMDC->getDisplayCapability();
}

}; // namespace intel
}; // namespace android
