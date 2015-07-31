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

#include <display/IMultiDisplayCallbackRegistrar.h>

namespace android {
namespace intel {


enum {
    MDS_SERVER_REGISTER_CALLBACK = IBinder::FIRST_CALL_TRANSACTION,
    MDS_SERVER_UNREGISTER_CALLBACK,
};

class BpMultiDisplayCallbackRegistrar:public BpInterface<IMultiDisplayCallbackRegistrar> {
public:
    BpMultiDisplayCallbackRegistrar(const sp<IBinder>& impl)
        : BpInterface<IMultiDisplayCallbackRegistrar>(impl)
    {
    }

    virtual status_t registerCallback(const sp<IMultiDisplayCallback>& cbk) {
        Parcel data, reply;
        data.writeInterfaceToken(IMultiDisplayCallbackRegistrar::getInterfaceDescriptor());
        data.writeStrongBinder(cbk->asBinder());
        status_t result = remote()->transact(
                MDS_SERVER_REGISTER_CALLBACK, data, &reply);
        if (result != NO_ERROR) {
            return result;
        }
        result = reply.readInt32();
        return result;
    }

    virtual status_t unregisterCallback(const sp<IMultiDisplayCallback>& cbk) {
        Parcel data, reply;
        data.writeInterfaceToken(IMultiDisplayCallbackRegistrar::getInterfaceDescriptor());
        data.writeStrongBinder(cbk->asBinder());
        status_t result = remote()->transact(
                MDS_SERVER_UNREGISTER_CALLBACK, data, &reply);
        if (result != NO_ERROR) {
            return result;
        }
        result = reply.readInt32();
        return result;
    }
};

IMPLEMENT_META_INTERFACE(MultiDisplayCallbackRegistrar,"com.intel.MultiDisplayCallbackRegistrar");

status_t BnMultiDisplayCallbackRegistrar::onTransact(
    uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags)
{
    switch (code) {
        case MDS_SERVER_REGISTER_CALLBACK: {
            CHECK_INTERFACE(IMultiDisplayCallbackRegistrar, data, reply);
            sp<IBinder> cbk = data.readStrongBinder();
            status_t ret = registerCallback(
                    interface_cast<IMultiDisplayCallback>(cbk));
            reply->writeInt32(ret);
            return NO_ERROR;
        } break;
        case MDS_SERVER_UNREGISTER_CALLBACK: {
            CHECK_INTERFACE(IMultiDisplayCallbackRegistrar, data, reply);
            sp<IBinder> cbk = data.readStrongBinder();
            status_t ret = unregisterCallback(
                    interface_cast<IMultiDisplayCallback>(cbk));
            reply->writeInt32(ret);
            return NO_ERROR;
        } break;
        default:
            return BBinder::onTransact(code, data, reply, flags);
    } // switch
}

}; // namespace intel
}; // namespace android
