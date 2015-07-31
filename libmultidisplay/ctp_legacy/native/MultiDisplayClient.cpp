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
#include <display/MultiDisplayType.h>
#include <display/MultiDisplayClient.h>
#include <display/IMultiDisplayComposer.h>
#include <display/MultiDisplayService.h>

namespace android {
namespace intel {


#define MDC_CHECK_IMDC() \
do { \
    if (mIMDComposer == NULL) { \
        ALOGE("%s: IMDSComposer is null", __func__); \
        return MDS_ERROR; \
    } \
} while(0)

MultiDisplayClient::MultiDisplayClient() {
    mIMDComposer = NULL;
    sp<IServiceManager> sm = defaultServiceManager();
    if (sm == NULL) {
        ALOGE("%s: Fail to get service manager", __func__);
        return;
    }
    sp<IBinder> service = sm->getService(String16(INTEL_MDS_SERVICE_NAME));
    if (service == NULL) {
        ALOGE("%s: Fail to get MDS service", __func__);
        return;
    } else
        mIMDComposer = interface_cast<IMultiDisplayComposer>(service);

}

MultiDisplayClient::~MultiDisplayClient() {
    ALOGV("MDSClient is destroyed, %p", this);
    mIMDComposer = NULL;
}

int MultiDisplayClient::setModePolicy(int policy) {
    MDC_CHECK_IMDC();
    return mIMDComposer->setModePolicy(policy);
}

int MultiDisplayClient::getDisplayMode(bool wait) {
    MDC_CHECK_IMDC();
    return mIMDComposer->getMode(wait);
}

int MultiDisplayClient::notifyWidi(bool on) {
    MDC_CHECK_IMDC();
    return mIMDComposer->notifyWidi(on);
}

int MultiDisplayClient::notifyMipi(bool on) {
    MDC_CHECK_IMDC();
    return mIMDComposer->notifyMipi(on);
}

MDS_VIDEO_STATE MultiDisplayClient::getVideoState(int sessionId) {
    if (mIMDComposer == NULL)
        return MDS_VIDEO_UNPREPARED;
    int state = mIMDComposer->getVideoState();
    if (state == MDS_ERROR)
        return MDS_VIDEO_UNPREPARED;
    return (MDS_VIDEO_STATE)state;
}

int MultiDisplayClient::resetVideoPlayback() {
    MDC_CHECK_IMDC();
    mIMDComposer->prepareForVideo(MDS_VIDEO_UNPREPARED);
    MDSVideoSourceInfo info;
    memset(&info, 0, sizeof(MDSVideoSourceInfo));
    return mIMDComposer->updateVideoInfo(info);
}

int MultiDisplayClient::allocateVideoSessionId() {
    MDC_CHECK_IMDC();
    return 0;
}

int MultiDisplayClient::updateVideoState(int sessionId, int status) {
    MDC_CHECK_IMDC();
    return mIMDComposer->prepareForVideo(status);
}

int MultiDisplayClient::updateVideoSourceInfo(int sessionId, const MDSVideoSourceInfo& info) {
    MDC_CHECK_IMDC();
    return mIMDComposer->updateVideoInfo(info);
}

int MultiDisplayClient::notifyHotPlug() {
    MDC_CHECK_IMDC();
    return mIMDComposer->notifyHotPlug();
}

int MultiDisplayClient::setHdmiPowerOff() {
    MDC_CHECK_IMDC();
    return mIMDComposer->setHdmiPowerOff();
}

int MultiDisplayClient::registerListener(
        sp<IExtendDisplayListener> listener, char* client, int msg) {
    MDC_CHECK_IMDC();
    return mIMDComposer->registerListener(listener,
            static_cast<void *>(this), client, msg);
}

int MultiDisplayClient::unregisterListener() {
    MDC_CHECK_IMDC();
    return mIMDComposer->unregisterListener(static_cast<void *>(this));
}


int MultiDisplayClient::getHdmiModeInfo(int* width, int* height, int* refresh, int* interlace, int *ratio) {
    MDC_CHECK_IMDC();
    return mIMDComposer->getHdmiModeInfo(width, height, refresh, interlace, ratio);
}

int MultiDisplayClient::setHdmiModeInfo(int width, int height, int refresh, int interlace, int ratio) {
    MDC_CHECK_IMDC();
    return mIMDComposer->setHdmiModeInfo(width, height, refresh, interlace, ratio);
}

int MultiDisplayClient::setHdmiScaleType(int type) {
    MDC_CHECK_IMDC();
    return mIMDComposer->setHdmiScaleType(type);
}

int MultiDisplayClient::setHdmiScaleStep(int hValue, int vValue) {
    MDC_CHECK_IMDC();
    return mIMDComposer->setHdmiScaleStep(hValue, vValue);
}

int MultiDisplayClient::getHdmiDeviceChange() {
    MDC_CHECK_IMDC();
    return mIMDComposer->getHdmiDeviceChange();
}

int MultiDisplayClient::getVideoInfo(int* displayW, int* displayH, int* fps, int* interlace) {
    MDC_CHECK_IMDC();
    return mIMDComposer->getVideoInfo(displayW, displayH, fps, interlace);
}

int MultiDisplayClient::getDisplayCapability() {
    MDC_CHECK_IMDC();
    return mIMDComposer->getDisplayCapability();
}

}; // namespace intel
}; // namespace android

