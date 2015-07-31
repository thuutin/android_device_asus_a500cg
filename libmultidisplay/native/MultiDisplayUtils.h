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



namespace android {
namespace intel {


#define MDC_CHECK_OBJECT(OBJECT, ERR) \
do { \
    if (OBJECT == NULL) { \
        ALOGE("%s: a non-instantiated object", __func__); \
        return ERR; \
    } \
} while(0)

#define IMPLEMENT_API_0(CLASS, OBJECT, INTERFACE, RETURN, ERR)               \
    RETURN CLASS::INTERFACE() {                         \
        MDC_CHECK_OBJECT(OBJECT, ERR);                                         \
        return OBJECT->INTERFACE();                            \
    }

#define IMPLEMENT_API_1(CLASS, OBJECT, INTERFACE, PARAM0, RETURN, ERR)       \
    RETURN CLASS::INTERFACE(PARAM0 p0) {                \
        MDC_CHECK_OBJECT(OBJECT, ERR);                                         \
        return OBJECT->INTERFACE(p0);                          \
    }

#define IMPLEMENT_API_2(CLASS, OBJECT, INTERFACE, PARAM0, PARAM1, RETURN, ERR)          \
    RETURN CLASS::INTERFACE(PARAM0 p0, PARAM1 p1) {                \
        MDC_CHECK_OBJECT(OBJECT, ERR);                                                    \
        return OBJECT->INTERFACE(p0, p1);                                 \
    }

#define IMPLEMENT_API_3(CLASS, OBJECT, INTERFACE, PARAM0, PARAM1, PARAM2, RETURN, ERR)  \
    RETURN CLASS::INTERFACE(PARAM0 p0, PARAM1 p1, PARAM2 p2) {     \
        MDC_CHECK_OBJECT(OBJECT, ERR);                                                    \
        return OBJECT->INTERFACE(p0, p1, p2);                             \
    }

#define IMPLEMENT_API_4(CLASS, OBJECT, INTERFACE, PARAM0, PARAM1, PARAM2, PARAM3, RETURN, ERR)  \
    RETURN CLASS::INTERFACE(PARAM0 p0, PARAM1 p1, PARAM2 p2, PARAM3 p3) {     \
        MDC_CHECK_OBJECT(OBJECT, ERR);                                                    \
        return OBJECT->INTERFACE(p0, p1, p2, p3);                             \
    }

}; // namespace intel
}; // namespace android
