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

#ifndef __ANDROID_ITNTEL_MULTIDISPLAYVIDEOCLIENT_H__
#define __ANDROID_ITNTEL_MULTIDISPLAYVIDEOCLIENT_H__

#include <utils/Errors.h>
#include <utils/RefBase.h>
#include <binder/IInterface.h>
#include <media/stagefright/foundation/AMessage.h>
#include <media/stagefright/MetaData.h>
#include <media/stagefright/MediaDefs.h>
#include <display/MultiDisplayClient.h>

using namespace android::intel;

namespace android {
namespace intel {

class MultiDisplayVideoClient : public RefBase {
private:
    int mSessionId;
    int mState;
    sp<MultiDisplayClient> mVideo;
    status_t prepare(int state, bool isProtected);
    void close();
public:
    MultiDisplayVideoClient();
    ~MultiDisplayVideoClient();
    status_t setVideoState(int state, bool isProtected, const sp<AMessage> &msg);
    status_t setVideoState(int state, bool isProtected, const sp<MetaData> &meta);
    status_t reset();
};

}; // namespace intel
}; // namespace android

#endif
