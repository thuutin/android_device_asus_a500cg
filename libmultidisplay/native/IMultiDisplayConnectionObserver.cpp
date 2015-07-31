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

#include <display/IMultiDisplayConnectionObserver.h>

namespace android {
namespace intel {


enum {
    MDS_SERVER_UPDATE_HDMI_CONNECTION_STATUS = IBinder::FIRST_CALL_TRANSACTION,
    MDS_SERVER_UPDATE_WIDI_CONNECTION_STATUS,
};

class BpMultiDisplayConnectionObserver : public BpInterface<IMultiDisplayConnectionObserver> {
public:
    BpMultiDisplayConnectionObserver(const sp<IBinder>& impl)
        : BpInterface<IMultiDisplayConnectionObserver>(impl)
    {
    }

    virtual status_t updateHdmiConnectionStatus(bool connected) {
        Parcel data, reply;
        data.writeInterfaceToken(IMultiDisplayConnectionObserver::getInterfaceDescriptor());
        data.writeInt32((connected ? 1 : 0));
        status_t result = remote()->transact(
                MDS_SERVER_UPDATE_HDMI_CONNECTION_STATUS, data, &reply);
        return result;
    }
    virtual status_t updateWidiConnectionStatus(bool connected) {
        Parcel data, reply;
        data.writeInterfaceToken(IMultiDisplayConnectionObserver::getInterfaceDescriptor());
        data.writeInt32((connected ? 1 : 0));
        status_t result = remote()->transact(
                MDS_SERVER_UPDATE_WIDI_CONNECTION_STATUS, data, &reply);
        return result;
    }
};

IMPLEMENT_META_INTERFACE(MultiDisplayConnectionObserver,"com.intel.MultiDisplayConnectionObserver");

status_t BnMultiDisplayConnectionObserver::onTransact(
    uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags)
{
    switch (code) {
        case MDS_SERVER_UPDATE_HDMI_CONNECTION_STATUS: {
            CHECK_INTERFACE(IMultiDisplayConnectionObserver, data, reply);
            bool state = (data.readInt32() == 1 ? true : false);
            status_t ret = updateHdmiConnectionStatus(state);
            reply->writeInt32(ret);
            return NO_ERROR;
        } break;
        case MDS_SERVER_UPDATE_WIDI_CONNECTION_STATUS: {
            CHECK_INTERFACE(IMultiDisplayConnectionObserver, data, reply);
            bool state = (data.readInt32() == 1 ? true : false);
            status_t ret = updateWidiConnectionStatus(state);
            reply->writeInt32(ret);
            return NO_ERROR;
        } break;
    } // switch
    return BBinder::onTransact(code, data, reply, flags);
}

}; // namespace intel
}; // namespace android
