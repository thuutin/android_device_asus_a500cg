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

#include <display/IMultiDisplayDecoderConfig.h>

namespace android {
namespace intel {


enum {
    MDS_SERVER_SET_DECODER_OUTPUT_RESOLUTION = IBinder::FIRST_CALL_TRANSACTION,
};

class BpMultiDisplayDecoderConfig:public BpInterface<IMultiDisplayDecoderConfig> {
public:
    BpMultiDisplayDecoderConfig(const sp<IBinder>& impl)
        : BpInterface<IMultiDisplayDecoderConfig>(impl)
    {
    }

    virtual status_t setDecoderOutputResolution(
            int videoSessionId,
            int32_t width, int32_t height,
            int32_t offX, int32_t offY,
            int32_t bufWidth, int32_t bufHeight) {
        Parcel data, reply;
        data.writeInterfaceToken(IMultiDisplayDecoderConfig::getInterfaceDescriptor());
        if (width <= 0 || height <= 0) {
            return BAD_VALUE;
        }
        data.writeInt32(videoSessionId);
        data.writeInt32(width);
        data.writeInt32(height);
        data.writeInt32(offX);
        data.writeInt32(offY);
        data.writeInt32(bufWidth);
        data.writeInt32(bufHeight);
        status_t result = remote()->transact(
                MDS_SERVER_SET_DECODER_OUTPUT_RESOLUTION, data, &reply);
        if (result != NO_ERROR) {
            return result;
        }
        result = reply.readInt32();
        return result;
    }
};

IMPLEMENT_META_INTERFACE(MultiDisplayDecoderConfig,"com.intel.MultiDisplayDecoderConfig");

status_t BnMultiDisplayDecoderConfig::onTransact(
    uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags)
{
    switch (code) {
        case MDS_SERVER_SET_DECODER_OUTPUT_RESOLUTION: {
            CHECK_INTERFACE(IMultiDisplayDecoderConfig, data, reply);
            int sessionId  = data.readInt32();
            int32_t width  = data.readInt32();
            int32_t height = data.readInt32();
            int32_t offX   = data.readInt32();
            int32_t offY   = data.readInt32();
            int32_t bufW   = data.readInt32();
            int32_t bufH   = data.readInt32();
            status_t ret = setDecoderOutputResolution(sessionId, width, height, offX, offY, bufW, bufH);
            reply->writeInt32(ret);
            return NO_ERROR;
        } break;
    } // switch
    return BBinder::onTransact(code, data, reply, flags);
}

}; // namespace intel
}; // namespace android
