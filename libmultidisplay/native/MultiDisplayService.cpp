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
#include <utils/Errors.h>

#include <binder/IServiceManager.h>
#include <binder/IPCThreadState.h>

#include <display/MultiDisplayService.h>
#include "MultiDisplayComposer.h"

namespace android {
namespace intel {

#define MDC_CHECK_OBJECT(OBJECT, ERR) \
do { \
    if (OBJECT == NULL) { \
        ALOGE("%s: a non-instantiated object", __func__); \
        return ERR; \
    } \
    ALOGV("Entering %s", __func__);\
} while(0)

#define IMPLEMENT_API_0(CLASS, OBJECT, INTERFACE, RETURN, ERR)               \
    RETURN CLASS::INTERFACE() {                         \
        MDC_CHECK_OBJECT(OBJECT, ERR);                                         \
        return OBJECT->INTERFACE();                            \
    }

#define IMPLEMENT_API_1(CLASS, OBJECT, INTERFACE, PARAM0, RETURN, ERR)       \
    RETURN CLASS::INTERFACE(PARAM0 p0) {                \
        MDC_CHECK_OBJECT(OBJECT, ERR);                                         \
        return OBJECT->INTERFACE(p0);                          \
    }

#define IMPLEMENT_API_2(CLASS, OBJECT, INTERFACE, PARAM0, PARAM1, RETURN, ERR)          \
    RETURN CLASS::INTERFACE(PARAM0 p0, PARAM1 p1) {                \
        MDC_CHECK_OBJECT(OBJECT, ERR);                                                    \
        return OBJECT->INTERFACE(p0, p1);                                 \
    }

#define IMPLEMENT_API_3(CLASS, OBJECT, INTERFACE, PARAM0, PARAM1, PARAM2, RETURN, ERR)  \
    RETURN CLASS::INTERFACE(PARAM0 p0, PARAM1 p1, PARAM2 p2) {     \
        MDC_CHECK_OBJECT(OBJECT, ERR);                                                    \
        return OBJECT->INTERFACE(p0, p1, p2);                             \
    }

#define IMPLEMENT_API_4(CLASS, OBJECT, INTERFACE, PARAM0, PARAM1, PARAM2, PARAM3, RETURN, ERR)  \
    RETURN CLASS::INTERFACE(PARAM0 p0, PARAM1 p1, PARAM2 p2, PARAM3 p3) {     \
        MDC_CHECK_OBJECT(OBJECT, ERR);                                                    \
        return OBJECT->INTERFACE(p0, p1, p2, p3);                             \
    }

#define IMPLEMENT_API_7(CLASS, OBJECT, INTERFACE, PARAM0, PARAM1, PARAM2, PARAM3, PARAM4, PARAM5, PARAM6, RETURN, ERR)  \
    RETURN CLASS::INTERFACE(PARAM0 p0, PARAM1 p1, PARAM2 p2, PARAM3 p3, PARAM4 p4, PARAM5 p5, PARAM6 p6) {     \
        MDC_CHECK_OBJECT(OBJECT, ERR);                                                    \
        return OBJECT->INTERFACE(p0, p1, p2, p3, p4, p5, p6);                             \
    }

class MultiDisplayHdmiControlImpl : public BnMultiDisplayHdmiControl {
private:
    sp<MultiDisplayComposer> pCom;
    static sp<MultiDisplayHdmiControlImpl> sHdmiInstance;
public:
    MultiDisplayHdmiControlImpl(const sp<MultiDisplayComposer>& com);
    int getCurrentHdmiTimingIndex();
    int getHdmiTimingCount();
    status_t setHdmiTiming(const MDSHdmiTiming&);
    status_t getHdmiTimingList(int, MDSHdmiTiming**);
    status_t getCurrentHdmiTiming(MDSHdmiTiming*);
    status_t setHdmiTimingByIndex(int);
    status_t setHdmiScalingType(MDS_SCALING_TYPE);
    status_t setHdmiOverscan(int, int);
    bool checkHdmiTimingIsFixed();
    static sp<MultiDisplayHdmiControlImpl> getInstance() {
        return sHdmiInstance;
    }
};

// singleton
sp<MultiDisplayHdmiControlImpl> MultiDisplayHdmiControlImpl::sHdmiInstance = NULL;

MultiDisplayHdmiControlImpl::MultiDisplayHdmiControlImpl(const sp<MultiDisplayComposer>& com) {
   pCom = com;
   sHdmiInstance = this;
}

IMPLEMENT_API_0(MultiDisplayHdmiControlImpl, pCom, getHdmiTimingCount, status_t, NO_INIT)
IMPLEMENT_API_0(MultiDisplayHdmiControlImpl, pCom, getCurrentHdmiTimingIndex, status_t, NO_INIT)
IMPLEMENT_API_0(MultiDisplayHdmiControlImpl, pCom, checkHdmiTimingIsFixed, bool, false)
IMPLEMENT_API_1(MultiDisplayHdmiControlImpl, pCom, setHdmiTiming, const MDSHdmiTiming&, status_t, NO_INIT)
IMPLEMENT_API_1(MultiDisplayHdmiControlImpl, pCom, getCurrentHdmiTiming, MDSHdmiTiming*, status_t, NO_INIT)
IMPLEMENT_API_1(MultiDisplayHdmiControlImpl, pCom, setHdmiScalingType, MDS_SCALING_TYPE, status_t, NO_INIT)
IMPLEMENT_API_1(MultiDisplayHdmiControlImpl, pCom, setHdmiTimingByIndex, int, status_t, NO_INIT)
IMPLEMENT_API_2(MultiDisplayHdmiControlImpl, pCom, setHdmiOverscan, int, int, status_t, NO_INIT)
IMPLEMENT_API_2(MultiDisplayHdmiControlImpl, pCom, getHdmiTimingList, int, MDSHdmiTiming**, status_t, NO_INIT)

// singleton
class MultiDisplayVideoControlImpl : public BnMultiDisplayVideoControl {
private:
    sp<MultiDisplayComposer> pCom;
    static sp<MultiDisplayVideoControlImpl> sVideoInstance;
public:
    MultiDisplayVideoControlImpl(const sp<MultiDisplayComposer>& com);
    int allocateVideoSessionId();
    status_t updateVideoState(int, MDS_VIDEO_STATE);
    status_t resetVideoPlayback();
    status_t updateVideoSourceInfo(int, const MDSVideoSourceInfo&);
    static sp<MultiDisplayVideoControlImpl> getInstance() {
        return sVideoInstance;
    }
};

// singleton
sp<MultiDisplayVideoControlImpl> MultiDisplayVideoControlImpl::sVideoInstance = NULL;

MultiDisplayVideoControlImpl::MultiDisplayVideoControlImpl(const sp<MultiDisplayComposer>& com) {
    pCom = com;
    sVideoInstance = this;
}

IMPLEMENT_API_0(MultiDisplayVideoControlImpl, pCom, resetVideoPlayback, status_t, NO_INIT)
IMPLEMENT_API_0(MultiDisplayVideoControlImpl, pCom, allocateVideoSessionId, int, -1)
IMPLEMENT_API_2(MultiDisplayVideoControlImpl, pCom, updateVideoState, int, MDS_VIDEO_STATE, status_t, NO_INIT)
IMPLEMENT_API_2(MultiDisplayVideoControlImpl, pCom, updateVideoSourceInfo, int, const MDSVideoSourceInfo&, status_t, NO_INIT)


class MultiDisplaySinkRegistrarImpl : public BnMultiDisplaySinkRegistrar {
private:
    sp<MultiDisplayComposer> pCom;
    static sp<MultiDisplaySinkRegistrarImpl> sSinkInstance;
public:
    MultiDisplaySinkRegistrarImpl(const sp<MultiDisplayComposer>& com);
    int32_t registerListener(const sp<IMultiDisplayListener>&, const char*, int);
    status_t unregisterListener(int32_t id);
    static sp<MultiDisplaySinkRegistrarImpl> getInstance() {
        return sSinkInstance;
    }
};

// singleton
sp<MultiDisplaySinkRegistrarImpl> MultiDisplaySinkRegistrarImpl::sSinkInstance = NULL;

MultiDisplaySinkRegistrarImpl::MultiDisplaySinkRegistrarImpl(const sp<MultiDisplayComposer>& com) {
    pCom = com;
    sSinkInstance = this;
}

IMPLEMENT_API_1(MultiDisplaySinkRegistrarImpl, pCom, unregisterListener, int32_t, status_t, NO_INIT)


IMPLEMENT_API_3(MultiDisplaySinkRegistrarImpl, pCom, registerListener, const sp<IMultiDisplayListener>&, const char*, int, int32_t, -1)

class MultiDisplayCallbackRegistrarImpl : public BnMultiDisplayCallbackRegistrar {
private:
    sp<MultiDisplayComposer> pCom;
    static sp<MultiDisplayCallbackRegistrarImpl> sCbInstance;
public:
    MultiDisplayCallbackRegistrarImpl(const sp<MultiDisplayComposer>& com);
    status_t registerCallback(const sp<IMultiDisplayCallback>&);
    status_t unregisterCallback(const sp<IMultiDisplayCallback>&);
    static sp<MultiDisplayCallbackRegistrarImpl> getInstance() {
        return sCbInstance;
    }
};

// singleton
sp<MultiDisplayCallbackRegistrarImpl> MultiDisplayCallbackRegistrarImpl::sCbInstance = NULL;

MultiDisplayCallbackRegistrarImpl::MultiDisplayCallbackRegistrarImpl(const sp<MultiDisplayComposer>& com) {
    pCom = com;
    sCbInstance = this;
}


IMPLEMENT_API_1(MultiDisplayCallbackRegistrarImpl, pCom, registerCallback,   const sp<IMultiDisplayCallback>&, status_t, NO_INIT)
IMPLEMENT_API_1(MultiDisplayCallbackRegistrarImpl, pCom, unregisterCallback, const sp<IMultiDisplayCallback>&, status_t, NO_INIT)

class MultiDisplayInfoProviderImpl : public BnMultiDisplayInfoProvider {
private:
    sp<MultiDisplayComposer> pCom;
    static sp<MultiDisplayInfoProviderImpl> sInfoInstance;
public:
    MultiDisplayInfoProviderImpl(const sp<MultiDisplayComposer>& com);
    MDS_VIDEO_STATE getVideoState(int);
    uint32_t getVppState();
    int getVideoSessionNumber();
    MDS_DISPLAY_MODE getDisplayMode(bool);
    status_t getVideoSourceInfo(int, MDSVideoSourceInfo*);
    status_t getDecoderOutputResolution(int, int32_t* width, int32_t* height, int32_t* offX, int32_t* offY, int32_t* bufW, int32_t* bufH);
    static sp<MultiDisplayInfoProviderImpl> getInstance() {
        return sInfoInstance;
    }
};

// singleton
sp<MultiDisplayInfoProviderImpl> MultiDisplayInfoProviderImpl::sInfoInstance = NULL;

MultiDisplayInfoProviderImpl::MultiDisplayInfoProviderImpl(const sp<MultiDisplayComposer>& com) {
    pCom = com;
    sInfoInstance = this;
}

IMPLEMENT_API_0(MultiDisplayInfoProviderImpl, pCom, getVideoSessionNumber, int, 0)
IMPLEMENT_API_0(MultiDisplayInfoProviderImpl, pCom, getVppState, uint32_t, false)
IMPLEMENT_API_1(MultiDisplayInfoProviderImpl, pCom, getVideoState, int,  MDS_VIDEO_STATE, MDS_VIDEO_STATE_UNKNOWN)
IMPLEMENT_API_1(MultiDisplayInfoProviderImpl, pCom, getDisplayMode, bool, MDS_DISPLAY_MODE,  MDS_MODE_NONE)
IMPLEMENT_API_2(MultiDisplayInfoProviderImpl, pCom, getVideoSourceInfo, int,  MDSVideoSourceInfo*, status_t, NO_INIT)
IMPLEMENT_API_7(MultiDisplayInfoProviderImpl, pCom, getDecoderOutputResolution, int, int32_t*, int32_t*, int32_t*, int32_t*, int32_t*, int32_t*, status_t, NO_INIT)

class MultiDisplayEventMonitorImpl : public BnMultiDisplayEventMonitor {
private:
    sp<MultiDisplayComposer> pCom;
    static sp<MultiDisplayEventMonitorImpl> sEventInstance;
public:
    MultiDisplayEventMonitorImpl(const sp<MultiDisplayComposer>& com);
    status_t updatePhoneCallState(bool);
    status_t updateInputState(bool);
    static sp<MultiDisplayEventMonitorImpl> getInstance() {
        return sEventInstance;
    }
};

// singleton
sp<MultiDisplayEventMonitorImpl> MultiDisplayEventMonitorImpl::sEventInstance = NULL;

MultiDisplayEventMonitorImpl::MultiDisplayEventMonitorImpl(const sp<MultiDisplayComposer>& com) {
    pCom = com;
    sEventInstance = this;
}

IMPLEMENT_API_1(MultiDisplayEventMonitorImpl, pCom, updatePhoneCallState,  bool,  status_t, NO_INIT)
IMPLEMENT_API_1(MultiDisplayEventMonitorImpl, pCom, updateInputState,      bool,  status_t, NO_INIT)

class MultiDisplayConnectionObserverImpl : public BnMultiDisplayConnectionObserver {
private:
    sp<MultiDisplayComposer> pCom;
    static sp<MultiDisplayConnectionObserverImpl> sConnInstance;
public:
    MultiDisplayConnectionObserverImpl(const sp<MultiDisplayComposer>& com);
    status_t updateWidiConnectionStatus(bool);
    status_t updateHdmiConnectionStatus(bool);
    static sp<MultiDisplayConnectionObserverImpl> getInstance() {
        return sConnInstance;
    }
};

// singleton
sp<MultiDisplayConnectionObserverImpl> MultiDisplayConnectionObserverImpl::sConnInstance = NULL;

MultiDisplayConnectionObserverImpl::MultiDisplayConnectionObserverImpl(const sp<MultiDisplayComposer>& com) {
    pCom = com;
    sConnInstance = this;
}

IMPLEMENT_API_1(MultiDisplayConnectionObserverImpl, pCom, updateWidiConnectionStatus, bool, status_t, NO_INIT)
IMPLEMENT_API_1(MultiDisplayConnectionObserverImpl, pCom, updateHdmiConnectionStatus, bool, status_t, NO_INIT)


class MultiDisplayDecoderConfigImpl : public BnMultiDisplayDecoderConfig {
private:
    sp<MultiDisplayComposer> pCom;
    static sp<MultiDisplayDecoderConfigImpl> sDecoderInstance;
public:
    MultiDisplayDecoderConfigImpl(const sp<MultiDisplayComposer>& com);
    status_t setDecoderOutputResolution(
            int videoSessionId,
            int32_t width, int32_t height,
            int32_t offX,  int32_t offY,
            int32_t bufW,  int32_t bufH);
    static sp<MultiDisplayDecoderConfigImpl> getInstance() {
        return sDecoderInstance;
    }
};

// singleton
sp<MultiDisplayDecoderConfigImpl> MultiDisplayDecoderConfigImpl::sDecoderInstance = NULL;

MultiDisplayDecoderConfigImpl::MultiDisplayDecoderConfigImpl(const sp<MultiDisplayComposer>& com) {
    pCom = com;
    sDecoderInstance = this;
}

IMPLEMENT_API_7(MultiDisplayDecoderConfigImpl, pCom, setDecoderOutputResolution, int, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, status_t, NO_INIT)

class MultiDisplayVppConfigImpl : public BnMultiDisplayVppConfig {
private:
    sp<MultiDisplayComposer> pCom;
    static sp<MultiDisplayVppConfigImpl> sVppInstance;
public:
    MultiDisplayVppConfigImpl(const sp<MultiDisplayComposer>& com);
    status_t setVppState(MDS_DISPLAY_ID, bool, int);
    static sp<MultiDisplayVppConfigImpl> getInstance() {
        return sVppInstance;
    }
};

// singleton
sp<MultiDisplayVppConfigImpl> MultiDisplayVppConfigImpl::sVppInstance = NULL;

MultiDisplayVppConfigImpl::MultiDisplayVppConfigImpl(const sp<MultiDisplayComposer>& com) {
    pCom = com;
    sVppInstance = this;
}

IMPLEMENT_API_3(MultiDisplayVppConfigImpl, pCom, setVppState, MDS_DISPLAY_ID, bool, int, status_t, NO_INIT)

enum {
    MDS_SERVICE_GET_HDMI_CONTROL = IBinder::FIRST_CALL_TRANSACTION,
    MDS_SERVICE_GET_VIDEO_CONTROL,
    MDS_SERVICE_GET_EVENT_MONITOR,
    MDS_SERVICE_GET_CALLBACK_REGISTRAR,
    MDS_SERVICE_GET_SINK_REGISTRAR,
    MDS_SERVICE_GET_INFO_PROVIDER,
    MDS_SERVICE_GET_CONNECTION_OBSERVER,
    MDS_SERVICE_GET_DECODER_CONFIG,
    MDS_SERVICE_GET_VPP_CONFIG,
};

class BpMDService : public BpInterface<IMDService> {
public:
    BpMDService(const sp<IBinder>& impl)
        : BpInterface<IMDService>(impl)
    {
    }

    virtual sp<IMultiDisplayHdmiControl> getHdmiControl() {
        Parcel data, reply;
        data.writeInterfaceToken(IMDService::getInterfaceDescriptor());
        status_t result = remote()->transact(
                MDS_SERVICE_GET_HDMI_CONTROL, data, &reply);
        if (result != NO_ERROR)
            ALOGE("Trasaction is fail");
        return interface_cast<IMultiDisplayHdmiControl>(reply.readStrongBinder());
    }

    virtual sp<IMultiDisplayVideoControl> getVideoControl() {
        Parcel data, reply;
        data.writeInterfaceToken(IMDService::getInterfaceDescriptor());
        status_t result = remote()->transact(
                MDS_SERVICE_GET_VIDEO_CONTROL, data, &reply);
        if (result != NO_ERROR)
            ALOGE("Trasaction is fail");
        return interface_cast<IMultiDisplayVideoControl>(reply.readStrongBinder());
    }

    virtual sp<IMultiDisplayEventMonitor> getEventMonitor() {
        Parcel data, reply;
        data.writeInterfaceToken(IMDService::getInterfaceDescriptor());
        status_t result = remote()->transact(
                MDS_SERVICE_GET_EVENT_MONITOR, data, &reply);
        if (result != NO_ERROR)
            ALOGE("Trasaction is fail");
        return interface_cast<IMultiDisplayEventMonitor>(reply.readStrongBinder());
    }

    virtual sp<IMultiDisplayCallbackRegistrar> getCallbackRegistrar() {
        Parcel data, reply;
        data.writeInterfaceToken(IMDService::getInterfaceDescriptor());
        status_t result = remote()->transact(
                MDS_SERVICE_GET_CALLBACK_REGISTRAR, data, &reply);
        if (result != NO_ERROR)
            ALOGE("Trasaction is fail");
        ALOGV("%s", __func__);
        return interface_cast<IMultiDisplayCallbackRegistrar>(reply.readStrongBinder());
    }

    virtual sp<IMultiDisplaySinkRegistrar> getSinkRegistrar() {
        Parcel data, reply;
        ALOGV("%s, %d", __func__, __LINE__);
        data.writeInterfaceToken(IMDService::getInterfaceDescriptor());
        status_t result = remote()->transact(
                MDS_SERVICE_GET_SINK_REGISTRAR, data, &reply);
        if (result != NO_ERROR)
            ALOGE("Trasaction is fail");
        ALOGV("%s, %d", __func__, __LINE__);
        return interface_cast<IMultiDisplaySinkRegistrar>(reply.readStrongBinder());
    }

    virtual sp<IMultiDisplayInfoProvider> getInfoProvider() {
        Parcel data, reply;
        data.writeInterfaceToken(IMDService::getInterfaceDescriptor());
        status_t result = remote()->transact(
                MDS_SERVICE_GET_INFO_PROVIDER, data, &reply);
        if (result != NO_ERROR)
            ALOGE("Trasaction is fail");
        return interface_cast<IMultiDisplayInfoProvider>(reply.readStrongBinder());
    }

    virtual sp<IMultiDisplayConnectionObserver> getConnectionObserver() {
        Parcel data, reply;
        data.writeInterfaceToken(IMDService::getInterfaceDescriptor());
        status_t result = remote()->transact(
                MDS_SERVICE_GET_CONNECTION_OBSERVER, data, &reply);
        if (result != NO_ERROR)
            ALOGE("Trasaction is fail");
        return interface_cast<IMultiDisplayConnectionObserver>(reply.readStrongBinder());
    }

    virtual sp<IMultiDisplayDecoderConfig> getDecoderConfig() {
        Parcel data, reply;
        data.writeInterfaceToken(IMDService::getInterfaceDescriptor());
        status_t result = remote()->transact(
                MDS_SERVICE_GET_DECODER_CONFIG, data, &reply);
        if (result != NO_ERROR)
            ALOGE("Trasaction is fail");
        return interface_cast<IMultiDisplayDecoderConfig>(reply.readStrongBinder());
    }
    virtual sp<IMultiDisplayVppConfig> getVppConfig() {
        Parcel data, reply;
        data.writeInterfaceToken(IMDService::getInterfaceDescriptor());
        status_t result = remote()->transact(
                MDS_SERVICE_GET_VPP_CONFIG, data, &reply);
        if (result != NO_ERROR)
            ALOGE("Trasaction is fail");
        return interface_cast<IMultiDisplayVppConfig>(reply.readStrongBinder());
    }
};

IMPLEMENT_META_INTERFACE(MDService,"com.intel.MDService");

status_t BnMDService::onTransact(
    uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags)
{
    switch (code) {
        case MDS_SERVICE_GET_HDMI_CONTROL: {
            CHECK_INTERFACE(IMDService, data, reply);
            sp<IBinder> b = this->getHdmiControl()->asBinder();
            reply->writeStrongBinder(b);
            return NO_ERROR;
        }
        case MDS_SERVICE_GET_VIDEO_CONTROL: {
            CHECK_INTERFACE(IMDService, data, reply);
            sp<IBinder> b = this->getVideoControl()->asBinder();
            reply->writeStrongBinder(b);
            return NO_ERROR;
        }
        case MDS_SERVICE_GET_EVENT_MONITOR: {
            CHECK_INTERFACE(IMDService, data, reply);
            sp<IBinder> b = this->getEventMonitor()->asBinder();
            reply->writeStrongBinder(b);
            return NO_ERROR;
        }
        case MDS_SERVICE_GET_CALLBACK_REGISTRAR: {
            CHECK_INTERFACE(IMDService, data, reply);
            sp<IBinder> b = this->getCallbackRegistrar()->asBinder();
            reply->writeStrongBinder(b);
            ALOGV("%s", __func__);
            return NO_ERROR;
        }
        case MDS_SERVICE_GET_SINK_REGISTRAR: {
            CHECK_INTERFACE(IMDService, data, reply);
            sp<IBinder> b = this->getSinkRegistrar()->asBinder();
            reply->writeStrongBinder(b);
            ALOGV("%s", __func__);
            return NO_ERROR;
        }
        case MDS_SERVICE_GET_INFO_PROVIDER: {
            CHECK_INTERFACE(IMDService, data, reply);
            sp<IBinder> b = this->getInfoProvider()->asBinder();
            reply->writeStrongBinder(b);
            return NO_ERROR;
        }
        case MDS_SERVICE_GET_CONNECTION_OBSERVER: {
            CHECK_INTERFACE(IMDService, data, reply);
            sp<IBinder> b = this->getConnectionObserver()->asBinder();
            reply->writeStrongBinder(b);
            return NO_ERROR;
        }
        case MDS_SERVICE_GET_DECODER_CONFIG: {
            CHECK_INTERFACE(IMDService, data, reply);
            sp<IBinder> b = this->getDecoderConfig()->asBinder();
            reply->writeStrongBinder(b);
            return NO_ERROR;
        }
        case MDS_SERVICE_GET_VPP_CONFIG: {
            CHECK_INTERFACE(IMDService, data, reply);
            sp<IBinder> b = this->getVppConfig()->asBinder();
            reply->writeStrongBinder(b);
            return NO_ERROR;
        }
        default:
            return BBinder::onTransact(code, data, reply, flags);
    } // switch
}


MultiDisplayService::MultiDisplayService() {
    ALOGI("%s: create a MultiDisplay service %p", __func__, this);
    sp<MultiDisplayComposer> com   = new MultiDisplayComposer();
    new MultiDisplayHdmiControlImpl(com);
    new MultiDisplayVideoControlImpl(com);
    new MultiDisplayEventMonitorImpl(com);
    new MultiDisplayCallbackRegistrarImpl(com);
    new MultiDisplaySinkRegistrarImpl(com);
    new MultiDisplayInfoProviderImpl(com);
    new MultiDisplayConnectionObserverImpl(com);
    new MultiDisplayDecoderConfigImpl(com);
    new MultiDisplayVppConfigImpl(com);
}

MultiDisplayService::~MultiDisplayService() {
    ALOGV("%s: MultiDisplay service %p is destoryed", __func__, this);
}

void MultiDisplayService::instantiate() {
    sp<IServiceManager> sm(defaultServiceManager());
    if (sm->addService(String16(INTEL_MDS_SERVICE_NAME),new MultiDisplayService()))
        ALOGE("Failed to start %s service", INTEL_MDS_SERVICE_NAME);
}

sp<IMultiDisplayHdmiControl> MultiDisplayService::getHdmiControl() {
	return MultiDisplayHdmiControlImpl::getInstance();
}

sp<IMultiDisplayVideoControl> MultiDisplayService::getVideoControl() {
	return MultiDisplayVideoControlImpl::getInstance();
}

sp<IMultiDisplayEventMonitor> MultiDisplayService::getEventMonitor() {
	return MultiDisplayEventMonitorImpl::getInstance();
}

sp<IMultiDisplayCallbackRegistrar> MultiDisplayService::getCallbackRegistrar() {
    ALOGV("%s", __func__);
	return MultiDisplayCallbackRegistrarImpl::getInstance();
}

sp<IMultiDisplaySinkRegistrar> MultiDisplayService::getSinkRegistrar() {
    ALOGV("%s", __func__);
	return MultiDisplaySinkRegistrarImpl::getInstance();
}

sp<IMultiDisplayInfoProvider> MultiDisplayService::getInfoProvider() {
	return MultiDisplayInfoProviderImpl::getInstance();
}

sp<IMultiDisplayConnectionObserver> MultiDisplayService::getConnectionObserver() {
	return MultiDisplayConnectionObserverImpl::getInstance();
}

sp<IMultiDisplayDecoderConfig> MultiDisplayService::getDecoderConfig() {
	return MultiDisplayDecoderConfigImpl::getInstance();
}

sp<IMultiDisplayVppConfig> MultiDisplayService::getVppConfig() {
	return MultiDisplayVppConfigImpl::getInstance();
}

}; //namespace intel
}; //namespace android
