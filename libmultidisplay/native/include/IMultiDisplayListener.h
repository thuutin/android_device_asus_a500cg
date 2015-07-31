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

#ifndef __IMULTIDISPLAY_LISTENER_H__
#define __IMULTIDISPLAY_LISTENER_H__

#include <utils/Errors.h>
#include <utils/RefBase.h>
#include <binder/IInterface.h>

#include <display/MultiDisplayType.h>

namespace android {
namespace intel {

/** @brief The messages MDS broadcasts to listeners */
typedef enum {
    MDS_MSG_MODE_CHANGE = 1 << 1,
} MDS_MESSAGE;

class IMultiDisplayListener : public IInterface
{
public:
    DECLARE_META_INTERFACE(MultiDisplayListener);

    // onMdsMessage is called by MultiDisplay Service
    // to notify the message to registered listeners.
    virtual status_t onMdsMessage(int msg, void* value, int size) = 0;
};

class BnMultiDisplayListener : public BnInterface<IMultiDisplayListener>
{
public:
    virtual status_t onTransact( uint32_t code,
                                 const Parcel& data,
                                 Parcel* reply,
                                 uint32_t flags = 0);

};

}; // namespace intel
}; // namespace android

#endif /* __IMULTIDISPLAY_LISTENER_H__ */
