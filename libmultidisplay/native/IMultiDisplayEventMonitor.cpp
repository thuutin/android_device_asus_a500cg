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


#include <display/IMultiDisplayEventMonitor.h>

namespace android {
namespace intel {


enum {
    MDS_SERVER_SET_PHONE_CALL_STATE = IBinder::FIRST_CALL_TRANSACTION,
    MDS_SERVER_SET_INPUT_STATE,
};

class BpMultiDisplayEventMonitor:public BpInterface<IMultiDisplayEventMonitor> {
public:
    BpMultiDisplayEventMonitor(const sp<IBinder>& impl)
        : BpInterface<IMultiDisplayEventMonitor>(impl)
    {
    }

    virtual status_t updatePhoneCallState(bool state) {
        Parcel data, reply;
        data.writeInterfaceToken(IMultiDisplayEventMonitor::getInterfaceDescriptor());
        data.writeInt32(state ? 1 : 0);
        status_t result = remote()->transact(
                MDS_SERVER_SET_PHONE_CALL_STATE, data, &reply);
        if (result != NO_ERROR) {
            return result;
        }
        result = reply.readInt32();
        return result;
    }

    virtual status_t updateInputState(bool state) {
        Parcel data, reply;
        data.writeInterfaceToken(IMultiDisplayEventMonitor::getInterfaceDescriptor());
        data.writeInt32(state ? 1 : 0);
        status_t result = remote()->transact(
                MDS_SERVER_SET_INPUT_STATE, data, &reply);
        if (result != NO_ERROR) {
            return result;
        }
        result = reply.readInt32();
        return result;
    }
};

IMPLEMENT_META_INTERFACE(MultiDisplayEventMonitor,"com.intel.MultiDisplayEventMonitor");

status_t BnMultiDisplayEventMonitor::onTransact(
    uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags)
{
    switch (code) {
        case MDS_SERVER_SET_PHONE_CALL_STATE: {
            CHECK_INTERFACE(IMultiDisplayEventMonitor, data, reply);
            bool state = (data.readInt32() == 1 ? true : false);
            status_t ret = updatePhoneCallState(state);
            reply->writeInt32(ret);
            return NO_ERROR;
        } break;
        case MDS_SERVER_SET_INPUT_STATE: {
            CHECK_INTERFACE(IMultiDisplayEventMonitor, data, reply);
            bool state = (data.readInt32() == 1 ? true : false);
            status_t ret = updateInputState(state);
            reply->writeInt32(ret);
            return NO_ERROR;
        } break;
    } // switch
    return BBinder::onTransact(code, data, reply, flags);
}

}; // namespace intel
}; // namespace android
