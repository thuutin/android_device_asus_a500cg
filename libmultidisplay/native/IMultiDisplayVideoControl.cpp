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


#include <display/IMultiDisplayVideoControl.h>

namespace android {
namespace intel {


enum {
    MDS_SERVER_ALLOCATE_VIDEO_SESSIONID = IBinder::FIRST_CALL_TRANSACTION,
    MDS_SERVER_RESET_VIDEO_PLAYBACK,
    MDS_SERVER_UPDATE_VIDEO_STATE,
    MDS_SERVER_UPDATE_VIDEO_SOURCE_INFO,
};

class BpMultiDisplayVideoControl : public BpInterface<IMultiDisplayVideoControl> {
public:
    BpMultiDisplayVideoControl(const sp<IBinder>& impl)
        : BpInterface<IMultiDisplayVideoControl>(impl)
    {
    }

    virtual status_t resetVideoPlayback() {
        Parcel data, reply;
        data.writeInterfaceToken(IMultiDisplayVideoControl::getInterfaceDescriptor());
        status_t result = remote()->transact(
                MDS_SERVER_RESET_VIDEO_PLAYBACK, data, &reply);
        if (result != NO_ERROR) {
            return result;
        }
        result = reply.readInt32();
        return result;
    }

    virtual int allocateVideoSessionId() {
        Parcel data, reply;
        data.writeInterfaceToken(IMultiDisplayVideoControl::getInterfaceDescriptor());
        status_t result = remote()->transact(
                MDS_SERVER_ALLOCATE_VIDEO_SESSIONID, data, &reply);
        if (result != NO_ERROR) {
            return -1;
        }
        return reply.readInt32();
    }

    virtual status_t updateVideoState(int sessionId, MDS_VIDEO_STATE state) {
        Parcel data, reply;
        data.writeInterfaceToken(IMultiDisplayVideoControl::getInterfaceDescriptor());
        data.writeInt32(sessionId);
        data.writeInt32(state);
        status_t result = remote()->transact(
                MDS_SERVER_UPDATE_VIDEO_STATE, data, &reply);
        if (result != NO_ERROR) {
            return result;
        }
        result = reply.readInt32();
        return result;
    }

    virtual status_t updateVideoSourceInfo(int sessionId, const MDSVideoSourceInfo& info) {
        Parcel data, reply;
        data.writeInterfaceToken(IMultiDisplayVideoControl::getInterfaceDescriptor());
        data.writeInt32(sessionId);
        data.write(&info, sizeof(MDSVideoSourceInfo));
        status_t result = remote()->transact(
                MDS_SERVER_UPDATE_VIDEO_SOURCE_INFO, data, &reply);
        if (result != NO_ERROR) {
            return result;
        }
        result = reply.readInt32();
        return result;
    }

};

IMPLEMENT_META_INTERFACE(MultiDisplayVideoControl,"com.intel.MultiDisplayVideoControl");

status_t BnMultiDisplayVideoControl::onTransact(
    uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags)
{
    switch (code) {
        case MDS_SERVER_ALLOCATE_VIDEO_SESSIONID: {
            CHECK_INTERFACE(IMultiDisplayVideoControl, data, reply);
            int32_t sessionId = allocateVideoSessionId();
            reply->writeInt32(sessionId);
            return NO_ERROR;
            } break;
        case MDS_SERVER_RESET_VIDEO_PLAYBACK: {
            CHECK_INTERFACE(IMultiDisplayVideoControl, data, reply);
            status_t ret = resetVideoPlayback();
            reply->writeInt32(ret);
            return NO_ERROR;
            } break;
        case MDS_SERVER_UPDATE_VIDEO_STATE: {
            CHECK_INTERFACE(IMultiDisplayVideoControl, data, reply);
            int32_t sessionId = data.readInt32();
            MDS_VIDEO_STATE state = (MDS_VIDEO_STATE)data.readInt32();
            status_t ret = updateVideoState(sessionId, state);
            reply->writeInt32(ret);
            return NO_ERROR;
        } break;
        case MDS_SERVER_UPDATE_VIDEO_SOURCE_INFO: {
            CHECK_INTERFACE(IMultiDisplayVideoControl, data, reply);
            int32_t sessionId = data.readInt32();
            MDSVideoSourceInfo info;
            data.read(&info, sizeof(MDSVideoSourceInfo));
            status_t ret = updateVideoSourceInfo(sessionId, info);
            reply->writeInt32(ret);
            return NO_ERROR;
        } break;
    } // switch
    return BBinder::onTransact(code, data, reply, flags);
}


}; // namespace intel
}; // namespace android
