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

#include <display/IMultiDisplayInfoProvider.h>

namespace android {
namespace intel {


enum {
    MDS_SERVER_GET_VIDEO_STATE = IBinder::FIRST_CALL_TRANSACTION,
    MDS_SERVER_GET_VIDEO_SESSION_NUM,
    MDS_SERVER_GET_VIDEO_SOURCE_INFO,
    MDS_SERVER_GET_DISPLAY_MODE,
    MDS_SERVER_GET_DECODER_OUTPUT_RESOLUTION,
    MDS_SERVER_GET_VPP_STATE,
};

class BpMultiDisplayInfoProvider:public BpInterface<IMultiDisplayInfoProvider> {
public:
    BpMultiDisplayInfoProvider(const sp<IBinder>& impl)
        : BpInterface<IMultiDisplayInfoProvider>(impl)
    {
    }

    virtual int getVideoSessionNumber() {
        Parcel data, reply;
        data.writeInterfaceToken(IMultiDisplayInfoProvider::getInterfaceDescriptor());
        status_t result = remote()->transact(
                MDS_SERVER_GET_VIDEO_SESSION_NUM, data, &reply);
        if (result != NO_ERROR) {
            return 0;
        }
        return reply.readInt32();
    }

    virtual MDS_VIDEO_STATE getVideoState(int sessionId) {
        Parcel data, reply;
        data.writeInterfaceToken(IMultiDisplayInfoProvider::getInterfaceDescriptor());
        data.writeInt32(sessionId);
        status_t result = remote()->transact(
                MDS_SERVER_GET_VIDEO_STATE, data, &reply);
        if (result != NO_ERROR) {
            return MDS_VIDEO_STATE_UNKNOWN;
        }
        MDS_VIDEO_STATE state = (MDS_VIDEO_STATE)reply.readInt32();
        return state;
    }

    virtual status_t getVideoSourceInfo(int sessionId, MDSVideoSourceInfo* info) {
        Parcel data, reply;
        data.writeInterfaceToken(IMultiDisplayInfoProvider::getInterfaceDescriptor());
        if (info == NULL) {
            return BAD_VALUE;
        }
        data.writeInt32(sessionId);
        status_t result = remote()->transact(
                MDS_SERVER_GET_VIDEO_SOURCE_INFO, data, &reply);
        if (result != NO_ERROR) {
            return result;
        }
        reply.read((void *)info, sizeof(MDSVideoSourceInfo));
        result = reply.readInt32();
        return result;
    }

    virtual MDS_DISPLAY_MODE getDisplayMode(bool wait) {
        Parcel data, reply;
        data.writeInterfaceToken(IMultiDisplayInfoProvider::getInterfaceDescriptor());
        data.writeInt32(wait ? 1 : 0);
        status_t result = remote()->transact(
                MDS_SERVER_GET_DISPLAY_MODE, data, &reply);
        if (result != NO_ERROR) {
            return MDS_MODE_NONE;
        }
        MDS_DISPLAY_MODE mode = (MDS_DISPLAY_MODE)reply.readInt32();
        return mode;
    }

    virtual status_t getDecoderOutputResolution(int sessionId,
            int32_t* width, int32_t* height,
            int32_t* offX, int32_t* offY,
            int32_t* bufW, int32_t* bufH) {
        Parcel data, reply;
        data.writeInterfaceToken(IMultiDisplayInfoProvider::getInterfaceDescriptor());
        if (width == NULL || height == NULL) {
            return BAD_VALUE;
        }
        data.writeInt32(sessionId);
        status_t result = remote()->transact(
                MDS_SERVER_GET_DECODER_OUTPUT_RESOLUTION, data, &reply);
        if (result != NO_ERROR) {
            return result;
        }
        *width  = reply.readInt32();
        *height = reply.readInt32();
        *offX   = reply.readInt32();
        *offY   = reply.readInt32();
        *bufW   = reply.readInt32();
        *bufH   = reply.readInt32();
        result  = reply.readInt32();
        return result;
    }

    virtual uint32_t getVppState() {
        Parcel data, reply;
        data.writeInterfaceToken(IMultiDisplayInfoProvider::getInterfaceDescriptor());
        status_t result = remote()->transact(
                MDS_SERVER_GET_VPP_STATE, data, &reply);
        if (result != NO_ERROR) {
            return 0;
        }
        return reply.readInt32();
    }
};

IMPLEMENT_META_INTERFACE(MultiDisplayInfoProvider,"com.intel.MultiDisplayInfoProvider");

status_t BnMultiDisplayInfoProvider::onTransact(
    uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags)
{
    switch (code) {
        case MDS_SERVER_GET_VIDEO_STATE: {
            CHECK_INTERFACE(IMultiDisplayInfoProvider, data, reply);
            int32_t sessionId = data.readInt32();
            MDS_VIDEO_STATE ret = getVideoState(sessionId);
            reply->writeInt32(ret);
            return NO_ERROR;
        } break;
        case MDS_SERVER_GET_VIDEO_SESSION_NUM: {
            CHECK_INTERFACE(IMultiDisplayInfoProvider, data, reply);
            int ret = getVideoSessionNumber();
            reply->writeInt32(ret);
            return NO_ERROR;
        } break;
        case MDS_SERVER_GET_VIDEO_SOURCE_INFO: {
            CHECK_INTERFACE(IMultiDisplayInfoProvider, data, reply);
            MDSVideoSourceInfo info;
            int32_t sessionId = data.readInt32();
            status_t ret = getVideoSourceInfo(sessionId, &info);
            reply->write((const void *)&info, sizeof(MDSVideoSourceInfo));
            reply->writeInt32(ret);
            return NO_ERROR;
        } break;
        case MDS_SERVER_GET_DISPLAY_MODE: {
            CHECK_INTERFACE(IMultiDisplayInfoProvider, data, reply);
            bool wait = (data.readInt32() == 0 ? false : true);
            MDS_DISPLAY_MODE ret = getDisplayMode(wait);
            reply->writeInt32(ret);
            return NO_ERROR;
        } break;
        case MDS_SERVER_GET_DECODER_OUTPUT_RESOLUTION: {
            CHECK_INTERFACE(IMultiDisplayInfoProvider, data, reply);
            int32_t width  = 0;
            int32_t height = 0;
            int32_t offX   = 0;
            int32_t offY   = 0;
            int32_t bufW   = 0;
            int32_t bufH   = 0;
            int32_t sessionId = data.readInt32();
            status_t ret = getDecoderOutputResolution(sessionId,
                    &width, &height, &offX, &offY, &bufW, &bufH);
            reply->writeInt32(width);
            reply->writeInt32(height);
            reply->writeInt32(offX);
            reply->writeInt32(offY);
            reply->writeInt32(bufW);
            reply->writeInt32(bufH);
            reply->writeInt32(ret);
            return NO_ERROR;
        } break;
        case MDS_SERVER_GET_VPP_STATE: {
            CHECK_INTERFACE(IMultiDisplayInfoProvider, data, reply);
            uint32_t ret = getVppState();
            reply->writeInt32(ret);
            return NO_ERROR;
        } break;
    } // switch
    return BBinder::onTransact(code, data, reply, flags);
}

}; // namespace intel
}; // namespace android
