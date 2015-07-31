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

#include <display/IMultiDisplayListener.h>


namespace android {
namespace intel {


enum {
    ON_MDS_EVENT = IBinder::FIRST_CALL_TRANSACTION,
};

class BpMultiDisplayListener : public BpInterface<IMultiDisplayListener>
{
public:
    BpMultiDisplayListener(const sp<IBinder>& impl)
        : BpInterface<IMultiDisplayListener>(impl)
    {
    }

    virtual status_t onMdsMessage(int msg, void* value, int size) {
        Parcel data, reply;
        data.writeInterfaceToken(IMultiDisplayListener::getInterfaceDescriptor());
        if (value == NULL || (uint32_t)size < sizeof(int))
            return BAD_VALUE;

        data.writeInt32(msg);
        data.writeInt32(size);
        data.write(value, size);

        ALOGV("%s: mode %d, 0x%x", __func__, msg, *((int*)value));

        status_t result = remote()->transact(ON_MDS_EVENT, data, &reply);
        if (result != NO_ERROR) {
            return result;
        }
        result = reply.readInt32();
        return result;
    }
};

IMPLEMENT_META_INTERFACE(MultiDisplayListener, "com.intel.MultiDisplayListener");

status_t BnMultiDisplayListener::onTransact(
    uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags)
{
    switch (code) {
        case ON_MDS_EVENT: {
            ALOGV("%s", __func__);
            CHECK_INTERFACE(IMultiDisplayListener, data, reply);
            int32_t msg = data.readInt32();
            int32_t size = data.readInt32();
            void* value = (void *)malloc(size);
            if (value == NULL)
                return NO_MEMORY;
            data.read(value, size);
            ALOGV("%s: mode %d, 0x%x", __func__, msg, *((int*)value));
            status_t ret = onMdsMessage(msg, value, size);
            reply->writeInt32(ret);
            if (value) {
                free(value);
                value = NULL;
            }
            return NO_ERROR;
       } break;
    }
    return BBinder::onTransact(code, data, reply, flags);
}

}; // namespace intel
}; // namespace android
