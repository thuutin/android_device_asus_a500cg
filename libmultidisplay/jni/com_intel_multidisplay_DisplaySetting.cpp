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
 */


//#define LOG_NDEBUG 0
#include "JNIHelp.h"
#include "jni.h"
#include <android_runtime/AndroidRuntime.h>

#include <utils/Log.h>
#include <utils/Errors.h>
#include <utils/threads.h>

#include <binder/Parcel.h>
#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>

#include <display/MultiDisplayService.h>

namespace android {
namespace intel {


#define CLASS_PATH_NAME  "com/intel/multidisplay/DisplaySetting"

sp<IMDService>  gMds = NULL;
static Mutex    gMutex;
static sp<class JNIMDSListener>    gListener = NULL;
static int32_t  gListenerId = -1;


class JNIMDSListener : public BnMultiDisplayListener
{
public:
    JNIMDSListener(JNIEnv* env, jobject thiz, jobject serviceObj);
    ~JNIMDSListener();
    status_t onMdsMessage(int msg, void* value, int size);

private:
    jobject mServiceObj; // reference to DisplaySetting Java object to call back
    jmethodID mOnMdsMessageMethodID; // onMdsMessage method id
};

JNIMDSListener::JNIMDSListener(JNIEnv* env, jobject thiz, jobject serviceObj)
{
    ALOGI("Creating JNI MDS listener.");
    jclass clazz = env->FindClass(CLASS_PATH_NAME);
    mOnMdsMessageMethodID = NULL;
    if (clazz == NULL) {
        ALOGE("%s: Fail to find class %s", __func__, CLASS_PATH_NAME);
    } else {
        mOnMdsMessageMethodID = env->GetMethodID(clazz, "onMdsMessage", "(II)V");
        if (mOnMdsMessageMethodID == NULL) {
            ALOGE("%s: Fail to find onMdsMessage method.", __func__);
        }
    }

    mServiceObj  = env->NewGlobalRef(serviceObj);
    if (!mServiceObj) {
        ALOGE("%s: Fail to reference serviceObj!", __func__);
    }
}

JNIMDSListener::~JNIMDSListener() {
    ALOGI("%s: Releasing MDS listener.", __func__);
    JNIEnv *env = AndroidRuntime::getJNIEnv();
    if (env) {
        // remove global reference
        env->DeleteGlobalRef(mServiceObj);
    }
}

status_t JNIMDSListener::onMdsMessage(int msg, void* value, int size)
{
    JNIEnv *env = AndroidRuntime::getJNIEnv();
    if (env == NULL) {
        ALOGE("%s: Faild to get JNI Env.", __func__);
        return NO_INIT;
    }

    if (!mServiceObj || !mOnMdsMessageMethodID) {
        ALOGE("%s: Invalid service object or method ID", __func__);
        return NO_INIT;
    }

    if (msg == (int)MDS_MSG_MODE_CHANGE) {
        ALOGV("Get a MDS mode change message %d, 0x%x", msg, *((int*)value));
        env->CallVoidMethod(mServiceObj, mOnMdsMessageMethodID, (int)msg, *((int*)value));
    }

    if (env->ExceptionCheck()) {
        ALOGW("%s: Exception occurred while posting message.", __func__);
        env->ExceptionClear();
    }

    return NO_ERROR;
}

static jboolean MDS_InitMDSClient(JNIEnv* env, jobject thiz, jobject serviceObj)
{
    AutoMutex _l(gMutex);
    if (env == NULL || thiz == NULL || serviceObj == NULL) {
        ALOGE("%s: Invalid jvm parameters.", __func__);
        return false;
    }

    sp<IServiceManager> sm = defaultServiceManager();
    if (sm == NULL) {
        ALOGE("%s: Fail to get service manager", __func__);
        return false;
    }
    gMds = interface_cast<IMDService>(sm->getService(String16(INTEL_MDS_SERVICE_NAME)));
    if (gMds == NULL) {
        ALOGE("%s: Failed to get MDS service", __func__);
        return false;
    }

    gListener = new JNIMDSListener(env, thiz, serviceObj);
    if (gListener == NULL) {
        ALOGE("%s: Failed to create JNIMDSListener instance.", __func__);
        return false;
    }
    sp<IMultiDisplaySinkRegistrar> sinkRegistrar = NULL;
    if ((sinkRegistrar = gMds->getSinkRegistrar()) == NULL)
        return false;
    gListenerId = sinkRegistrar->registerListener(gListener,
            "DisplaySetting", MDS_MSG_MODE_CHANGE);
    ALOGV("MDS JNI listener ID %d", gListenerId);
    return true;
}

static jboolean MDS_DeInitMDSClient(JNIEnv* env, jobject obj)
{
    AutoMutex _l(gMutex);
    sp<IMultiDisplaySinkRegistrar> sinkRegistrar = NULL;
    if (gListenerId >= 0 && gListener != NULL && gMds != NULL &&
            (sinkRegistrar = gMds->getSinkRegistrar()) != NULL) {
        sinkRegistrar->unregisterListener(gListenerId);
    }
    gListenerId = -1;
    gListener   = NULL;
    ALOGI("%s: Release MultiDisplay JNI client.", __func__);
    return true;
}

static jint MDS_getMode(JNIEnv* env, jobject obj)
{
    AutoMutex _l(gMutex);
    if (gMds == NULL) return 0;
    sp<IMultiDisplayInfoProvider> infoProvider = gMds->getInfoProvider();
    if (infoProvider == NULL) return 0;
    return infoProvider->getDisplayMode(true);
}

static jint MDS_getHdmiTiming(
    JNIEnv* env,
    jobject obj,
    jintArray width,
    jintArray height,
    jintArray refresh,
    jintArray interlace,
    jintArray ratio)
{
    AutoMutex _l(gMutex);
    if (gMds == NULL) return 0;
    sp<IMultiDisplayHdmiControl> hdmiControl = gMds->getHdmiControl();
    if (hdmiControl == NULL) return 0;
    int32_t* pWidth = env->GetIntArrayElements(width, NULL);
    int32_t* pHeight = env->GetIntArrayElements(height, NULL);
    int32_t* pRefresh = env->GetIntArrayElements(refresh, NULL);
    int32_t* pInterlace = env->GetIntArrayElements(interlace, NULL);
    int32_t* pRatio = env->GetIntArrayElements(ratio, NULL);
    // The total supported timing count
    jint iCount = hdmiControl->getHdmiTimingCount();

    if (iCount > 0) {
        jint i,j;
        MDSHdmiTiming *list[iCount];
        memset(list, 0, iCount * sizeof(MDSHdmiTiming*));
        for (i = 0; i < iCount; i++) {
            list[i] = (MDSHdmiTiming *)malloc(sizeof(MDSHdmiTiming));
            if (list[i] == NULL) {
                for (j = 0; j < i; j++)
                    if (list[j]) free(list[j]);
                return 0;
            }
            memset(list[i], 0, sizeof(MDSHdmiTiming));
        }

        hdmiControl->getHdmiTimingList(iCount, list);
        for (i = 0; i < iCount; i++) {
            pRatio[i]     = list[i]->ratio;
            pWidth[i]     = list[i]->width;
            pHeight[i]    = list[i]->height;
            pRefresh[i]   = list[i]->refresh;
            pInterlace[i] = list[i]->interlace;
        }

        for (i = 0; i < iCount; i++) {
            if (list[i]) free(list[i]);
        }
    }

    env->ReleaseIntArrayElements(width, pWidth, 0);
    env->ReleaseIntArrayElements(height, pHeight, 0);
    env->ReleaseIntArrayElements(refresh, pRefresh, 0);
    env->ReleaseIntArrayElements(interlace, pInterlace, 0);
    env->ReleaseIntArrayElements(ratio, pRatio, 0);
    return iCount;
}

static jboolean MDS_setHdmiTiming(
    JNIEnv* env,
    jobject obj,
    jint width,
    jint height,
    jint refresh,
    jint interlace,
    jint ratio)
{
    AutoMutex _l(gMutex);
    if (gMds == NULL) return false;
    sp<IMultiDisplayHdmiControl> hdmiControl = gMds->getHdmiControl();
    if (hdmiControl == NULL) return 0;

    MDSHdmiTiming timing;
    timing.ratio = ratio;
    timing.width = width;
    timing.height = height;
    timing.refresh = refresh;
    timing.interlace = interlace;

    status_t ret = hdmiControl->setHdmiTiming(timing);
    return (ret == NO_ERROR ? true : false);
}

static jint MDS_getHdmiInfoCount(JNIEnv* env, jobject obj)
{
    AutoMutex _l(gMutex);
    if (gMds == NULL) return 0;
    sp<IMultiDisplayHdmiControl> hdmiControl = gMds->getHdmiControl();
    if (hdmiControl == NULL) return 0;
    return hdmiControl->getHdmiTimingCount();
}

static jboolean MDS_setHdmiScaleType(JNIEnv* env, jobject obj, jint type)
{
    AutoMutex _l(gMutex);
    if (gMds == NULL) return false;
    sp<IMultiDisplayHdmiControl> hdmiControl = gMds->getHdmiControl();
    if (hdmiControl == NULL) return false;
    status_t ret = hdmiControl->setHdmiScalingType((MDS_SCALING_TYPE)type);
    return (ret == NO_ERROR ? true : false);
}

static jboolean MDS_setHdmiOverscan(JNIEnv* env, jobject obj, jint hValue, jint vValue)
{
    AutoMutex _l(gMutex);
    if (gMds == NULL) return false;
    sp<IMultiDisplayHdmiControl> hdmiControl = gMds->getHdmiControl();
    if (hdmiControl == NULL) return false;
    status_t ret = hdmiControl->setHdmiOverscan(hValue, vValue);
    return (ret == NO_ERROR ? true : false);
}

static jint MDS_updatePhoneCallState(JNIEnv* env, jobject obj, jboolean state)
{
    AutoMutex _l(gMutex);
    if (gMds == NULL) return 0;
    sp<IMultiDisplayEventMonitor> eventMonitor = gMds->getEventMonitor();
    if (eventMonitor == NULL) return 0;
    return eventMonitor->updatePhoneCallState(state);
}

static jint MDS_updateInputState(JNIEnv* env, jobject obj, jboolean state)
{
    AutoMutex _l(gMutex);
    if (gMds == NULL) return 0;
    sp<IMultiDisplayEventMonitor> eventMonitor = gMds->getEventMonitor();
    if (eventMonitor == NULL) return 0;
    return eventMonitor->updateInputState(state);
}

static jint MDS_setVppState(JNIEnv* env, jobject obj, int dpyId, jboolean state, int status)
{
#ifdef TARGET_HAS_ISV
    AutoMutex _l(gMutex);
    if (gMds == NULL) return 0;
    sp<IMultiDisplayVppConfig> vppConfig = gMds->getVppConfig();
    if (vppConfig == NULL) return 0;
    return vppConfig->setVppState((MDS_DISPLAY_ID)dpyId, state, status);
#else
    return 0;
#endif
}

static JNINativeMethod sMethods[] = {
    /* name, signature, funcPtr */
    {"native_InitMDSClient", "(Lcom/intel/multidisplay/DisplaySetting;)Z", (void*)MDS_InitMDSClient},
    {"native_DeInitMDSClient", "()Z", (void*)MDS_DeInitMDSClient},
    {"native_getMode", "()I", (void*)MDS_getMode},
    {"native_setHdmiTiming", "(IIIII)Z", (void*)MDS_setHdmiTiming},
    {"native_getHdmiTiming", "([I[I[I[I[I)I", (void*)MDS_getHdmiTiming},
    {"native_getHdmiInfoCount", "()I", (void*)MDS_getHdmiInfoCount},
    {"native_setHdmiScaleType", "(I)Z", (void*)MDS_setHdmiScaleType},
    {"native_setHdmiOverscan", "(II)Z", (void*)MDS_setHdmiOverscan},
    {"native_updatePhoneCallState", "(Z)I", (void*)MDS_updatePhoneCallState},
    {"native_updateInputState", "(Z)I", (void*)MDS_updateInputState},
    {"native_setVppState", "(IZI)I", (void*)MDS_setVppState},
};

int register_intel_multidisplay_DisplaySetting(JNIEnv* env)
{
    ALOGV("Entering %s", __func__);
    jclass clazz = env->FindClass(CLASS_PATH_NAME);
    if (clazz == NULL) {
        ALOGE("%s: Fail to find class %s", __func__, CLASS_PATH_NAME);
        return -1;
    }
    int ret = jniRegisterNativeMethods(env, CLASS_PATH_NAME, sMethods, NELEM(sMethods));
    ALOGV("Leaving %s, return = %d", __func__, ret);
    return ret;
}

}; // namespace intel
}; // namespace android
