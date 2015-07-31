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
#include <binder/Parcel.h>

#include <display/IMultiDisplaySinkRegistrar.h>

namespace android {
namespace intel {


enum {
    MDS_SERVER_REGISTER_LISTENER = IBinder::FIRST_CALL_TRANSACTION,
    MDS_SERVER_UNREGISTER_LISTENER,
};

class BpMultiDisplaySinkRegistrar : public BpInterface<IMultiDisplaySinkRegistrar> {
public:
    BpMultiDisplaySinkRegistrar(const sp<IBinder>& impl)
        : BpInterface<IMultiDisplaySinkRegistrar>(impl)
    {
    }

    virtual int32_t registerListener(const sp<IMultiDisplayListener>& listener,
            const char* name, int msg) {
        Parcel data, reply;
        if (listener.get() == NULL || name == NULL) {
            return -1;
        }
        data.writeInterfaceToken(IMultiDisplaySinkRegistrar::getInterfaceDescriptor());
        data.writeStrongBinder(listener->asBinder());
        data.writeCString(name);
        data.writeInt32(msg);
        status_t result = remote()->transact(
                MDS_SERVER_REGISTER_LISTENER, data, &reply);
        if (result != NO_ERROR) {
            return -1;
        }
        int32_t id = reply.readInt32();
        ALOGV("%s, %d, %p", __func__, id, listener.get());
        return id;
    }

    virtual status_t unregisterListener(int32_t listenerId) {
        Parcel data, reply;
        if (listenerId < 0) {
            return BAD_VALUE;
        }
        ALOGV("%s, %d", __func__, listenerId);
        data.writeInterfaceToken(IMultiDisplaySinkRegistrar::getInterfaceDescriptor());
        data.writeInt32(listenerId);
        status_t result = remote()->transact(
                MDS_SERVER_UNREGISTER_LISTENER, data, &reply);
        if (result != NO_ERROR) {
            return result;
        }
        result = reply.readInt32();
        return result;
    }
};

IMPLEMENT_META_INTERFACE(MultiDisplaySinkRegistrar,"com.intel.MultiDisplaySinkRegistrar");

status_t BnMultiDisplaySinkRegistrar::onTransact(
    uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags)
{
    switch (code) {
        case MDS_SERVER_REGISTER_LISTENER: {
            CHECK_INTERFACE(IMultiDisplaySinkRegistrar, data, reply);
            sp<IMultiDisplayListener> listener =
                interface_cast<IMultiDisplayListener>(data.readStrongBinder());
            const char* client = data.readCString();
            int msg = data.readInt32();
            int32_t listenerId = registerListener(listener, client, msg);
            ALOGV("%s, %d, %p", __func__, listenerId, listener.get());
            reply->writeInt32(listenerId);
            return NO_ERROR;
        } break;
        case MDS_SERVER_UNREGISTER_LISTENER: {
            CHECK_INTERFACE(IMultiDisplaySinkRegistrar, data, reply);
            int32_t listenerId = data.readInt32();
            ALOGV("%s, %d", __func__, listenerId);
            status_t ret = unregisterListener(listenerId);
            reply->writeInt32(ret);
            return NO_ERROR;
        } break;
        default:
            return BBinder::onTransact(code, data, reply, flags);
    } // switch
}

}; // namespace intel
}; // namespace android
