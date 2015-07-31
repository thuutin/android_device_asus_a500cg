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

#include <display/IMultiDisplayVppConfig.h>

namespace android {
namespace intel {


enum {
    MDS_SERVER_SET_VPP_STATE = IBinder::FIRST_CALL_TRANSACTION,
};

class BpMultiDisplayVppConfig:public BpInterface<IMultiDisplayVppConfig> {
public:
    BpMultiDisplayVppConfig(const sp<IBinder>& impl)
        : BpInterface<IMultiDisplayVppConfig>(impl)
    {
    }

    virtual status_t setVppState(MDS_DISPLAY_ID dpyId, bool isOn, int status) {
        Parcel data, reply;
        data.writeInterfaceToken(IMultiDisplayVppConfig::getInterfaceDescriptor());
        data.writeInt32(dpyId);
        data.writeInt32(isOn ? 1 : 0);
        data.writeInt32(status);
        status_t result = remote()->transact(
                MDS_SERVER_SET_VPP_STATE, data, &reply);
        if (result != NO_ERROR) {
            return result;
        }
        result = reply.readInt32();
        return result;
    }
};

IMPLEMENT_META_INTERFACE(MultiDisplayVppConfig,"com.intel.MultiDisplayVppConfig");

status_t BnMultiDisplayVppConfig::onTransact(
    uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags)
{
    switch (code) {
        case MDS_SERVER_SET_VPP_STATE: {
            CHECK_INTERFACE(IMultiDisplayVppConfig, data, reply);
            MDS_DISPLAY_ID dpyId = (MDS_DISPLAY_ID)(data.readInt32());
            bool isOn  = (data.readInt32() == 1 ? true : false);
            int status = data.readInt32();
            status_t ret = setVppState(dpyId, isOn, status);
            reply->writeInt32(ret);
            return NO_ERROR;
        } break;
    } // switch
    return BBinder::onTransact(code, data, reply, flags);
}

}; // namespace intel
}; // namespace android
