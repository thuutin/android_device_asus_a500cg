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

#ifndef __IEXTEND_DISPLAY_LISTENER_H__
#define __IEXTEND_DISPLAY_LISTENER_H__

#include <utils/RefBase.h>
#include <binder/IInterface.h>
#include <binder/Parcel.h>

namespace android {
namespace intel {

class IExtendDisplayListener : public IInterface {
public:
    enum {
        ON_MDS_EVENT = IBinder::FIRST_CALL_TRANSACTION,
    };
    DECLARE_META_INTERFACE(ExtendDisplayListener);
    virtual int onMdsMessage(int msg, void* value, int size) = 0;
};
class BpExtendDisplayListener : public BpInterface<IExtendDisplayListener> {
public:
    BpExtendDisplayListener(const sp<IBinder>& impl)
        : BpInterface<IExtendDisplayListener>(impl) {}
    virtual int onMdsMessage(int msg, void* value, int size);
};

class BnExtendDisplayListener : public BnInterface<IExtendDisplayListener> {
public:
    virtual status_t onTransact(uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags = 0);
};

}; // namespace intel
}; // namespace android

#endif /* __IEXTEND_DISPLAY_LISTENER_H__ */
