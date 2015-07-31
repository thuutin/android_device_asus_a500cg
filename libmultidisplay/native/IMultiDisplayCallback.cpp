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

#include <display/IMultiDisplayCallback.h>

namespace android {
namespace intel {


enum {
    MDS_CB_BLANK_SECONDARY_DISPLAY = IBinder::FIRST_CALL_TRANSACTION,
    MDS_CB_UPDATE_VIDEO_PLAYBACK_STATE,
    MDS_CB_SET_HDMI_TIMING,
    MDS_CB_SET_HDMI_SCALING_TYPE,
    MDS_CB_SET_HDMI_OVERSCAN,
    MDS_CB_SET_INPUT_STATE,
};

class BpMultiDisplayCallback : public BpInterface<IMultiDisplayCallback>
{
public:
    BpMultiDisplayCallback(const sp<IBinder>& impl)
        : BpInterface<IMultiDisplayCallback>(impl)
    {
    }

    virtual status_t blankSecondaryDisplay(bool blank) {
        Parcel data, reply;
        data.writeInterfaceToken(IMultiDisplayCallback::getInterfaceDescriptor());
        data.writeInt32(blank ? 1 : 0);
        status_t result = remote()->transact(
                MDS_CB_BLANK_SECONDARY_DISPLAY, data, &reply);
        if (result != NO_ERROR) {
            return result;
        }
        result = reply.readInt32();
        return result;
    }

    virtual status_t updateVideoState(int videoSessionId, MDS_VIDEO_STATE state) {
        Parcel data, reply;
        data.writeInterfaceToken(IMultiDisplayCallback::getInterfaceDescriptor());
        data.writeInt32(videoSessionId);
        data.writeInt32(state);
        status_t result = remote()->transact(
                MDS_CB_UPDATE_VIDEO_PLAYBACK_STATE, data, &reply);
        if (result != NO_ERROR) {
            return result;
        }
        result = reply.readInt32();
        return result;
    }

    virtual status_t setHdmiTiming(const MDSHdmiTiming& timing) {
        Parcel data, reply;
        data.writeInterfaceToken(IMultiDisplayCallback::getInterfaceDescriptor());

        data.write(&timing, sizeof(MDSHdmiTiming));
        status_t result = remote()->transact(
                MDS_CB_SET_HDMI_TIMING, data, &reply);
        if (result != NO_ERROR) {
            return result;
        }
        result = reply.readInt32();
        return result;
    }
    virtual status_t setHdmiScalingType(MDS_SCALING_TYPE type) {
        Parcel data, reply;
        data.writeInterfaceToken(IMultiDisplayCallback::getInterfaceDescriptor());
        data.writeInt32(type);
        status_t result = remote()->transact(
                MDS_CB_SET_HDMI_SCALING_TYPE, data, &reply);
        if (result != NO_ERROR) {
            return result;
        }
        result = reply.readInt32();
        return result;
    }

    virtual status_t setHdmiOverscan(int hValue, int vValue) {
        Parcel data, reply;
        data.writeInterfaceToken(IMultiDisplayCallback::getInterfaceDescriptor());
        data.writeInt32(hValue);
        data.writeInt32(vValue);
        status_t result = remote()->transact(
                MDS_CB_SET_HDMI_OVERSCAN, data, &reply);
        if (result != NO_ERROR) {
            return result;
        }
        result = reply.readInt32();
        return result;
    }

    virtual status_t updateInputState(bool state) {
        Parcel data, reply;
        data.writeInterfaceToken(IMultiDisplayCallback::getInterfaceDescriptor());
        data.writeInt32(state ? 1 : 0);
        status_t result = remote()->transact(
                MDS_CB_SET_INPUT_STATE, data, &reply);
        if (result != NO_ERROR) {
            return result;
        }
        result = reply.readInt32();
        return result;
    }

};

IMPLEMENT_META_INTERFACE(MultiDisplayCallback, "com.intel.MultiDisplayCallback");

status_t BnMultiDisplayCallback::onTransact(
    uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags)
{
    switch (code) {
        case MDS_CB_BLANK_SECONDARY_DISPLAY: {
            CHECK_INTERFACE(IMultiDisplayCallback, data, reply);
            int32_t blank = data.readInt32();
            ALOGV("%s: Set phone state %d", __func__, blank);
            int32_t ret = blankSecondaryDisplay(blank ? true : false);
            reply->writeInt32(ret);
            return NO_ERROR;
        } break;
        case MDS_CB_UPDATE_VIDEO_PLAYBACK_STATE: {
            CHECK_INTERFACE(IMultiDisplayCallback, data, reply);
            int32_t vsessionId = data.readInt32();
            int32_t state = data.readInt32();
            ALOGV("%s: Set video playback[%d] state %d", __func__,  vsessionId, state);
            int32_t ret = updateVideoState(vsessionId, (MDS_VIDEO_STATE)state);
            reply->writeInt32(ret);
            return NO_ERROR;
        } break;
        case MDS_CB_SET_HDMI_TIMING: {
            CHECK_INTERFACE(IMultiDisplayCallback, data, reply);
            MDSHdmiTiming timing;
            data.read(&timing, sizeof(MDSHdmiTiming));
            ALOGV("%s: set HDMI timing, %dx%d@%dx%dx%d", __func__,
                    timing.width, timing.height, timing.refresh, timing.ratio);
            int32_t ret = setHdmiTiming(timing);
            reply->writeInt32(ret);
            return NO_ERROR;
        } break;
        case MDS_CB_SET_HDMI_SCALING_TYPE: {
            CHECK_INTERFACE(IMultiDisplayCallback, data, reply);
            int32_t type = data.readInt32();
            ALOGV("%s: set HDMI scaling type to:%d", __func__, type);
            int32_t ret = setHdmiScalingType((MDS_SCALING_TYPE)type);
            reply->writeInt32(ret);
            return NO_ERROR;
        } break;
        case MDS_CB_SET_HDMI_OVERSCAN: {
            CHECK_INTERFACE(IMultiDisplayCallback, data, reply);
            int32_t hValue = data.readInt32();
            int32_t vValue = data.readInt32();
            ALOGV("%s: set HDMI overscan h:%d v:%d", __func__,  hValue, vValue);
            int32_t ret = setHdmiOverscan(hValue, vValue);
            reply->writeInt32(ret);
            return NO_ERROR;
        } break;
        case MDS_CB_SET_INPUT_STATE: {
            CHECK_INTERFACE(IMultiDisplayCallback, data, reply);
            bool state = (data.readInt32() == 1 ? true : false);
            ALOGV("%s: Set input state %d", __func__, state);
            int32_t ret = updateInputState(state);
            reply->writeInt32(ret);
            return NO_ERROR;
        } break;
    }
    return BBinder::onTransact(code, data, reply, flags);
}

}; // namespace intel
}; // namespace android
