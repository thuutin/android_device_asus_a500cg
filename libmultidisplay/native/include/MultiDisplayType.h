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

#ifndef __MULTIDISPLAYTYPE_H__
#define __MULTIDISPLAYTYPE_H__

#include <hardware/hwcomposer_defs.h>


namespace android {
namespace intel {


static const int MDS_VIDEO_SESSION_MAX_VALUE = 16;

/** @brief The display ID */
typedef enum {
    MDS_DISPLAY_PRIMARY   = HWC_DISPLAY_PRIMARY,
    MDS_DISPLAY_EXTERNAL  = HWC_DISPLAY_EXTERNAL,
    MDS_DISPLAY_VIRTUAL   = HWC_DISPLAY_VIRTUAL,
} MDS_DISPLAY_ID;

/** @brief Video source info from video players */
typedef struct {
    int  frameRate;    /**< frame rate */
    int  displayW;     /**< displayed width */
    int  displayH;     /**< displayed height */
    bool isInterlaced; /**< true: interlaced false: progressive */
    bool isProtected;  /**< protected content */
} MDSVideoSourceInfo;

/** @brief HDMI timing structure */
typedef struct {
    int         width;
    int         height;
    uint32_t    refresh;    /**< refresh rate */
    int         interlace;  /**< 1:interlaced 0:progressive */
    int         ratio;      /**< aspect ratio */
    uint32_t    flags;      /**< expended flag */
} MDSHdmiTiming;

/** @brief The state of video playback
 * 2 state machines for clear content and proteced content
 * Clear content:     UNPREPARED->PREPARED->UNPREPARED->UNPREPARED
 * Protected content: UNPREPARED->PREPARING->PREPARED->
 *                      UNPREPAARING->UNPREPARED->UNPREPARED
 */
typedef enum {
    MDS_VIDEO_STATE_UNKNOWN = 0,
    MDS_VIDEO_PREPARING     = 1,
    MDS_VIDEO_PREPARED      = 2,
    MDS_VIDEO_UNPREPARING   = 3,
    MDS_VIDEO_UNPREPARED    = 4,
} MDS_VIDEO_STATE;

/** @brief The scaling type @see SurfaceFligner.h */
typedef enum {
    MDS_SCALING_NONE        = 0,
    MDS_SCALING_FULL_SCREEN = 1,
    MDS_SCALING_CENTER      = 2,
    MDS_SCALING_ASPECT      = 3,
} MDS_SCALING_TYPE;


}; // namespace intel
}; // namespace android
#endif
