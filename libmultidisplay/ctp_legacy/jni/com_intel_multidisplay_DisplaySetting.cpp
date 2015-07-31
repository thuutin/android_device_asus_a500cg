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

#define LOG_TAG "MultiDisplay"

#include "JNIHelp.h"
#include "jni.h"
#include <android_runtime/AndroidRuntime.h>
#include <utils/Log.h>
#include <utils/threads.h>
#include <binder/Parcel.h>
#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>
#include <display/MultiDisplayClient.h>
#include <display/IExtendDisplayListener.h>

namespace android {
namespace intel {


#define CLASS_PATH_NAME  "com/intel/multidisplay/DisplaySetting"
static sp<class JNIMDSListener> gListener = NULL;
static MultiDisplayClient* gMDClient = NULL;
static Mutex gMutex;


class JNIMDSListener : public BnExtendDisplayListener
{
public:
    JNIMDSListener(JNIEnv* env, jobject thiz, jobject serviceObj);
    ~JNIMDSListener();
    int onMdsMessage(int msg, void* value, int size);

private:
    JNIMDSListener(); // private constructor
    jobject mServiceObj; // reference to DisplaySetting Java object to call back
    jmethodID mOnMdsMessageMethodID; // onMdsMessage method id
};

JNIMDSListener::JNIMDSListener(JNIEnv* env, jobject thiz, jobject serviceObj)
{
    ALOGI("%s: Creating MDS listener.", __func__);
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

int JNIMDSListener::onMdsMessage(int msg, void* value, int size)
{
    ALOGV("Entering %s", __func__);

    JNIEnv *env = AndroidRuntime::getJNIEnv();
    if (env == NULL) {
        ALOGE("%s: Faild to get JNI Env.", __func__);
        return MDS_ERROR;
    }

    if (!mServiceObj || !mOnMdsMessageMethodID) {
        ALOGE("%s: Invalid service object or method ID", __func__);
        return MDS_ERROR;
    }

    if (msg == MDS_MODE_CHANGE && size == sizeof(int)) {
        ALOGV("%s: Get message from MDS, %d, 0x%x", __func__, msg, *((int*)value));
        env->CallVoidMethod(mServiceObj, mOnMdsMessageMethodID, msg, *((int*)value));
    }

    if (env->ExceptionCheck()) {
        ALOGW("%s: Exception occurred while posting message.", __func__);
        env->ExceptionClear();
    }

    ALOGV("Leaving %s", __func__);
    return MDS_NO_ERROR;
}

static jboolean MDS_InitMDSClient(JNIEnv* env, jobject thiz, jobject serviceObj)
{
    AutoMutex _l(gMutex);
    ALOGI("%s: creating MultiDisplay JNI client.", __func__);
    if (gMDClient) {
        ALOGW("%s: MultiDisplay JNI client has been created.", __func__);
        return true;
    }

    if (env == NULL || thiz == NULL || serviceObj == NULL) {
        ALOGE("%s: Invalid input parameters.", __func__);
        return false;
    }

    gMDClient = new MultiDisplayClient();
    if (gMDClient == NULL) {
        ALOGE("%s: Failed to create MultiDisplayClient instance.", __func__);
        return false;
    }

    gListener = new JNIMDSListener(env, thiz, serviceObj);
    if (gListener == NULL) {
        ALOGE("%s: Failed to create JNIMDSListener instance.", __func__);
        delete gMDClient;
        gMDClient = NULL;
        return false;
    }

    gMDClient->registerListener(gListener, "DisplaySetting", MDS_MODE_CHANGE);
    return true;
}

static jboolean MDS_DeInitMDSClient(JNIEnv* env, jobject obj)
{
    AutoMutex _l(gMutex);
    ALOGI("%s: Releasing MultiDisplay JNI client.", __func__);
    if (gListener != NULL && gMDClient != NULL) {
        gMDClient->unregisterListener();
        gListener = NULL;
        delete gMDClient;
        gMDClient = NULL;
    }
    return true;
}

static jint MDS_getMode(JNIEnv* env, jobject obj)
{
    if (gMDClient == NULL) return 0;
    AutoMutex _l(gMutex);
    return gMDClient->getDisplayMode(true);
}

static jboolean MDS_setModePolicy(JNIEnv* env, jobject obj, jint policy)
{
    if (gMDClient == NULL) return false;
    AutoMutex _l(gMutex);
    int ret = gMDClient->setModePolicy(policy);
    return (ret == MDS_NO_ERROR ? true : false);
}

static jboolean MDS_notifyHotPlug(JNIEnv* env, jobject obj)
{
    if (gMDClient == NULL) return false;
    AutoMutex _l(gMutex);
    int ret = gMDClient->notifyHotPlug();
    return (ret == MDS_NO_ERROR ? true : false);
}

static jboolean MDS_setHdmiPowerOff(JNIEnv* env, jobject obj)
{
    if (gMDClient == NULL) return false;
    AutoMutex _l(gMutex);
    int ret = gMDClient->setHdmiPowerOff();
    return (ret == 0 ? true : false);
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
    if (gMDClient == NULL) return 0;
    AutoMutex _l(gMutex);
    int32_t* pWidth = env->GetIntArrayElements(width, NULL);
    int32_t* pHeight = env->GetIntArrayElements(height, NULL);
    int32_t* pRefresh = env->GetIntArrayElements(refresh, NULL);
    int32_t* pInterlace = env->GetIntArrayElements(interlace, NULL);
    int32_t* pRatio = env->GetIntArrayElements(ratio, NULL);
    jint iCount = gMDClient->getHdmiModeInfo(pWidth, pHeight, pRefresh, pInterlace, pRatio);
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
    if (gMDClient == NULL) return false;
    AutoMutex _l(gMutex);
    int ret = gMDClient->setHdmiModeInfo(width, height, refresh, interlace, ratio);
    return (ret == MDS_NO_ERROR ? true : false);
}

static jint MDS_getHdmiInfoCount(JNIEnv* env, jobject obj)
{
    if (gMDClient == NULL) return 0;
    AutoMutex _l(gMutex);
    return gMDClient->getHdmiModeInfo(NULL,NULL, NULL, NULL, NULL);
}

static jboolean MDS_HdmiScaleType(JNIEnv* env, jobject obj,jint Type)
{
    if (gMDClient == NULL) return false;
    AutoMutex _l(gMutex);
    int ret = gMDClient->setHdmiScaleType(Type);
    return (ret == 0 ? true : false);
}

static jboolean MDS_HdmiScaleStep(JNIEnv* env, jobject obj,jint hValue,jint vValue)
{
    if (gMDClient == NULL) return false;
    AutoMutex _l(gMutex);
    int ret = gMDClient->setHdmiScaleStep(hValue,vValue);
    return (ret == 0 ? true : false);
}

static jint MDS_getHdmiDeviceChange(JNIEnv* env, jobject obj)
{
    if (gMDClient == NULL) return 0;
    AutoMutex _l(gMutex);
    return gMDClient->getHdmiDeviceChange();
}

static jint MDS_getDisplayCapability(JNIEnv* env, jobject obj)
{
    if (gMDClient == NULL) return 0;
    AutoMutex _l(gMutex);
    return gMDClient->getDisplayCapability();
}

static JNINativeMethod sMethods[] = {
    /* name, signature, funcPtr */
    {"native_InitMDSClient", "(Lcom/intel/multidisplay/DisplaySetting;)Z", (void*)MDS_InitMDSClient},
    {"native_DeInitMDSClient", "()Z", (void*)MDS_DeInitMDSClient},
    {"native_getMode", "()I", (void*)MDS_getMode},
    {"native_setModePolicy", "(I)Z", (void*)MDS_setModePolicy},
    {"native_notifyHotPlug", "()Z", (void*)MDS_notifyHotPlug},
    {"native_setHdmiPowerOff", "()Z", (void*)MDS_setHdmiPowerOff},
    {"native_setHdmiTiming", "(IIIII)Z", (void*)MDS_setHdmiTiming},
    {"native_getHdmiTiming", "([I[I[I[I[I)I", (void*)MDS_getHdmiTiming},
    {"native_getHdmiInfoCount", "()I", (void*)MDS_getHdmiInfoCount},
    {"native_setHdmiScaleType", "(I)Z", (void*)MDS_HdmiScaleType},
    {"native_setHdmiScaleStep", "(II)Z", (void*)MDS_HdmiScaleStep},
    {"native_getHdmiDeviceChange", "()I", (void*)MDS_getHdmiDeviceChange},
    {"native_getDisplayCapability", "()I", (void*)MDS_getDisplayCapability},
};


int register_intel_multidisplay_DisplaySetting(JNIEnv* env)
{
    ALOGD("Entering %s", __func__);
    jclass clazz = env->FindClass(CLASS_PATH_NAME);
    if (clazz == NULL) {
        ALOGE("%s: Fail to find class %s", __func__, CLASS_PATH_NAME);
        return -1;
    }
    int ret = jniRegisterNativeMethods(env, CLASS_PATH_NAME, sMethods, NELEM(sMethods));
    ALOGD("Leaving %s, return = %d", __func__, ret);
    return ret;
}

}; // namespace intel
}; // namespace android
