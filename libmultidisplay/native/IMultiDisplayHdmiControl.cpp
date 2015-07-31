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
#include <utils/RefBase.h>
#include <binder/Parcel.h>

#include <display/IMultiDisplayHdmiControl.h>
#include "drm_hdmi.h"

namespace android {
namespace intel {

enum {
    MDS_SERVER_SET_HDMI_TIMING = IBinder::FIRST_CALL_TRANSACTION,
    MDS_SERVER_GET_HDMI_TIMING_COUNT,
    MDS_SERVER_GET_HDMI_TIMING_LIST,
    MDS_SERVER_SET_HDMI_TIMING_BY_INDEX,
    MDS_SERVER_GET_CURRENT_HDMI_TIMING,
    MDS_SERVER_GET_CURRENT_HDMI_TIMING_INDEX,
    MDS_SERVER_SET_HDMI_SCALING_TYPE,
    MDS_SERVER_SET_HDMI_OVER_SCAN,
    MDS_SERVER_CHECK_HDMI_TIMING_FIXED,
};

class BpMultiDisplayHdmiControl : public BpInterface<IMultiDisplayHdmiControl> {
public:
    BpMultiDisplayHdmiControl(const sp<IBinder>& impl)
        : BpInterface<IMultiDisplayHdmiControl>(impl)
    {
    }
    virtual status_t setHdmiTiming(const MDSHdmiTiming& timing) {
        Parcel data, reply;
        data.writeInterfaceToken(IMultiDisplayHdmiControl::getInterfaceDescriptor());
        data.write(&timing, sizeof(MDSHdmiTiming));
        status_t result = remote()->transact(
                MDS_SERVER_SET_HDMI_TIMING, data, &reply);
        if (result != NO_ERROR) {
            return result;
        }
        result = reply.readInt32();
        return result;
    }

    virtual int getHdmiTimingCount() {
        Parcel data, reply;
        data.writeInterfaceToken(IMultiDisplayHdmiControl::getInterfaceDescriptor());
        status_t result = remote()->transact(
                MDS_SERVER_GET_HDMI_TIMING_COUNT, data, &reply);
        if (result != NO_ERROR) {
            return result;
        }
        int count = reply.readInt32();
        return count;
    }

    virtual status_t getHdmiTimingList(int timingCount, MDSHdmiTiming** list) {
        Parcel data, reply;
        data.writeInterfaceToken(IMultiDisplayHdmiControl::getInterfaceDescriptor());
        if (list == NULL || timingCount <= 0 || timingCount > HDMI_TIMING_MAX) {
            return BAD_VALUE;
        }
        data.writeInt32(timingCount);
        status_t result = remote()->transact(
                MDS_SERVER_GET_HDMI_TIMING_LIST, data, &reply);
        if (result != NO_ERROR) {
            return result;
        }
        for (int i = 0; i < timingCount; i++) {
            if (list[i] == NULL)
                return BAD_VALUE;

            reply.read((void*)list[i], sizeof(MDSHdmiTiming));
        }
        result = reply.readInt32();
        return result;
    }

    virtual status_t getCurrentHdmiTiming(MDSHdmiTiming* timing) {
        Parcel data, reply;
        data.writeInterfaceToken(IMultiDisplayHdmiControl::getInterfaceDescriptor());
        if (timing == NULL) {
            return BAD_VALUE;
        }
        status_t result = remote()->transact(
                MDS_SERVER_GET_CURRENT_HDMI_TIMING, data, &reply);
        if (result != NO_ERROR) {
            return result;
        }
        reply.read((void *)timing, sizeof(MDSHdmiTiming));
        result = reply.readInt32();
        return result;
    }

    virtual status_t setHdmiTimingByIndex(int index) {
        Parcel data, reply;
        data.writeInterfaceToken(IMultiDisplayHdmiControl::getInterfaceDescriptor());
        data.writeInt32(index);
        status_t result = remote()->transact(
                MDS_SERVER_SET_HDMI_TIMING_BY_INDEX, data, &reply);
        if (result != NO_ERROR) {
            return result;
        }
        result = reply.readInt32();
        return result;
    }

    virtual int getCurrentHdmiTimingIndex() {
        Parcel data, reply;
        data.writeInterfaceToken(IMultiDisplayHdmiControl::getInterfaceDescriptor());
        status_t result = remote()->transact(
                MDS_SERVER_GET_CURRENT_HDMI_TIMING_INDEX, data, &reply);
        if (result != NO_ERROR) {
            return result;
        }
        int32_t index = reply.readInt32();
        return index;
    }

    virtual status_t setHdmiScalingType(MDS_SCALING_TYPE type) {
        Parcel data, reply;
        data.writeInterfaceToken(IMultiDisplayHdmiControl::getInterfaceDescriptor());
        data.writeInt32(type);
        status_t result = remote()->transact(
                MDS_SERVER_SET_HDMI_SCALING_TYPE, data, &reply);
        if (result != NO_ERROR) {
            return result;
        }
        result = reply.readInt32();
        return result;
    }

    virtual status_t setHdmiOverscan(int hValue, int vValue) {
        Parcel data, reply;
        data.writeInterfaceToken(IMultiDisplayHdmiControl::getInterfaceDescriptor());
        data.writeInt32(hValue);
        data.writeInt32(vValue);
        status_t result = remote()->transact(
                MDS_SERVER_SET_HDMI_OVER_SCAN, data, &reply);
        if (result != NO_ERROR) {
            return result;
        }
        result = reply.readInt32();
        return result;
    }

    virtual bool checkHdmiTimingIsFixed() {
        Parcel data, reply;
        data.writeInterfaceToken(IMultiDisplayHdmiControl::getInterfaceDescriptor());
        status_t result = remote()->transact(
                MDS_SERVER_CHECK_HDMI_TIMING_FIXED, data, &reply);
        if (result != NO_ERROR) {
            return false;
        }
        return (reply.readInt32() == 1 ? true : false);
    }
};

IMPLEMENT_META_INTERFACE(MultiDisplayHdmiControl,"com.intel.MultiDisplayHdmiControl");

status_t BnMultiDisplayHdmiControl::onTransact(
    uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags)
{
    switch (code) {
        case MDS_SERVER_SET_HDMI_TIMING: {
            CHECK_INTERFACE(IMultiDisplayHdmiControl, data, reply);
            MDSHdmiTiming timing;
            data.read(&timing, sizeof(MDSHdmiTiming));
            status_t ret = setHdmiTiming(timing);
            reply->writeInt32(ret);
            return NO_ERROR;
        } break;
        case MDS_SERVER_GET_HDMI_TIMING_COUNT: {
            CHECK_INTERFACE(IMultiDisplayHdmiControl, data, reply);
            int32_t ret = getHdmiTimingCount();
            reply->writeInt32(ret);
            return NO_ERROR;
        } break;
        case MDS_SERVER_GET_HDMI_TIMING_LIST: {
            CHECK_INTERFACE(IMultiDisplayHdmiControl, data, reply);
            const int count = data.readInt32();
            MDSHdmiTiming *list[count];
            memset(list, 0, count * sizeof(MDSHdmiTiming *));
            for (int i = 0; i < count; i++) {
                list[i] = (MDSHdmiTiming *)malloc(sizeof(MDSHdmiTiming));
                if (list[i] == NULL) {
                    for (int j = 0; j < i; j++)
                        if (list[j]) free(list[j]);
                    return NO_ERROR;
                }
                memset(list[i], 0, sizeof(MDSHdmiTiming));
            }

            int ret = getHdmiTimingList(count, (MDSHdmiTiming **)list);

            for (int i = 0; i < count; i++) {
                reply->write((const void*)list[i], sizeof(MDSHdmiTiming));
                free(list[i]);
                list[i] = NULL;
            }
            reply->writeInt32(ret);
            return NO_ERROR;
        } break;
        case MDS_SERVER_GET_CURRENT_HDMI_TIMING: {
            CHECK_INTERFACE(IMultiDisplayHdmiControl, data, reply);
            MDSHdmiTiming timing;
            status_t ret = getCurrentHdmiTiming(&timing);
            reply->write((const void *)&timing, sizeof(MDSHdmiTiming));
            reply->writeInt32(ret);
            return NO_ERROR;
        } break;
        case MDS_SERVER_SET_HDMI_TIMING_BY_INDEX: {
            CHECK_INTERFACE(IMultiDisplayHdmiControl, data, reply);
            int32_t index = data.readInt32();
            status_t ret = setHdmiTimingByIndex(index);
            reply->writeInt32(ret);
            return NO_ERROR;
        } break;
        case MDS_SERVER_GET_CURRENT_HDMI_TIMING_INDEX: {
            CHECK_INTERFACE(IMultiDisplayHdmiControl, data, reply);
            int32_t index = getCurrentHdmiTimingIndex();
            reply->writeInt32(index);
            return NO_ERROR;
        } break;
        case MDS_SERVER_SET_HDMI_SCALING_TYPE: {
            CHECK_INTERFACE(IMultiDisplayHdmiControl, data, reply);
            MDS_SCALING_TYPE type = (MDS_SCALING_TYPE)data.readInt32();
            status_t ret = setHdmiScalingType(type);
            reply->writeInt32(ret);
            return NO_ERROR;
        } break;
        case MDS_SERVER_SET_HDMI_OVER_SCAN: {
            CHECK_INTERFACE(IMultiDisplayHdmiControl, data, reply);
            int32_t hValue = data.readInt32();
            int32_t vValue = data.readInt32();
            status_t ret = setHdmiOverscan(hValue, vValue);
            reply->writeInt32(ret);
            return NO_ERROR;
        } break;
        case MDS_SERVER_CHECK_HDMI_TIMING_FIXED: {
            CHECK_INTERFACE(IMultiDisplayInfoProvider, data, reply);
            bool ret = checkHdmiTimingIsFixed();
            reply->writeInt32(ret ? 1 : 0);
            return NO_ERROR;
        } break;
    }
    return BBinder::onTransact(code, data, reply, flags);
}

}; // namespace intel
}; // namespace android
