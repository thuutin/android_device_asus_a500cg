# Author: tianyang.zhu@intel.com

LOCAL_PATH:= $(call my-dir)

#$(warning $(TARGET_HAS_MULTIPLE_DISPLAY))
#$(warning $(TARGET_BOARD_PLATFORM))

ifeq ($(USE_MDS_LEGACY),true)
include $(LOCAL_PATH)/ctp_legacy/Android.mk

else
ifeq ($(TARGET_HAS_MULTIPLE_DISPLAY),true)
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := display
LOCAL_COPY_HEADERS := \
    native/include/MultiDisplayType.h \
    native/include/IMultiDisplayListener.h \
    native/include/IMultiDisplayCallback.h \
    native/include/IMultiDisplayHdmiControl.h \
    native/include/IMultiDisplayVideoControl.h \
    native/include/IMultiDisplayEventMonitor.h \
    native/include/IMultiDisplaySinkRegistrar.h \
    native/include/IMultiDisplayCallbackRegistrar.h \
    native/include/IMultiDisplayConnectionObserver.h \
    native/include/IMultiDisplayInfoProvider.h \
    native/include/IMultiDisplayDecoderConfig.h \
    native/include/MultiDisplayService.h

LOCAL_COPY_HEADERS += videoclient/MultiDisplayVideoClient.h

ifeq ($(TARGET_HAS_ISV),true)
LOCAL_COPY_HEADERS += \
    native/include/IMultiDisplayVppConfig.h
endif


include $(BUILD_COPY_HEADERS)

include $(CLEAR_VARS)
LOCAL_SRC_FILES:= \
    native/MultiDisplayComposer.cpp \
    native/IMultiDisplayListener.cpp \
    native/IMultiDisplayCallback.cpp \
    native/IMultiDisplayInfoProvider.cpp \
    native/IMultiDisplayConnectionObserver.cpp \
    native/IMultiDisplayHdmiControl.cpp \
    native/IMultiDisplayVideoControl.cpp \
    native/IMultiDisplayEventMonitor.cpp \
    native/IMultiDisplaySinkRegistrar.cpp \
    native/IMultiDisplayCallbackRegistrar.cpp \
    native/IMultiDisplayDecoderConfig.cpp \
    native/MultiDisplayService.cpp
ifeq ($(TARGET_HAS_ISV),true)
LOCAL_SRC_FILES += native/IMultiDisplayVppConfig.cpp
endif

LOCAL_MODULE:= libmultidisplay
LOCAL_MODULE_TAGS := optional

LOCAL_SHARED_LIBRARIES := \
    libui libcutils libutils libbinder
LOCAL_CFLAGS := -DLOG_TAG=\"MultiDisplay\"

ifeq ($(ENABLE_IMG_GRAPHICS),true)
    LOCAL_SRC_FILES += \
        native/drm_hdmi.cpp

    LOCAL_C_INCLUDES = \
        $(TARGET_OUT_HEADERS)/libdrm \
        $(TARGET_OUT_HEADERS)/pvr/pvr2d \
        $(TARGET_OUT_HEADERS)/libttm

    LOCAL_SHARED_LIBRARIES += libdrm

    LOCAL_CFLAGS += -DENABLE_DRM
    LOCAL_CFLAGS += -DDVI_SUPPORTED
    LOCAL_SHARED_LIBRARIES += libdl
endif

ifeq ($(ENABLE_GEN_GRAPHICS),true)
    LOCAL_SRC_FILES += \
        native/drm_hdmi.cpp

    LOCAL_C_INCLUDES = \
        $(TARGET_OUT_HEADERS)/libdrm \
        $(TARGET_OUT_HEADERS)/external/drm

    LOCAL_SHARED_LIBRARIES += libdrm

    LOCAL_CFLAGS += -DDVI_SUPPORTED -DVPG_DRM
endif

ifeq ($(TARGET_HAS_ISV),true)
LOCAL_CFLAGS += -DTARGET_HAS_ISV
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/libmedia_utils_vpp
LOCAL_SHARED_LIBRARIES += libvpp_setting
endif

LOCAL_C_INCLUDES += $(call include-path-for, frameworks-av)

#LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)

include $(BUILD_SHARED_LIBRARY)

# Build MDS Video Client static library
include $(CLEAR_VARS)

LOCAL_SRC_FILES := videoclient/MultiDisplayVideoClient.cpp

LOCAL_MODULE := libmultidisplayvideoclient
LOCAL_MODULE_TAGS :=optional

LOCAL_SHARED_LIBRARIES += libmultidisplay

LOCAL_C_INCLUDES := videoclient/MultiDisplayVideoClient.h \
     $(call include-path-for, frameworks-av)

LOCAL_CFLAGS += -DLOG_TAG=\"MultiDisplay\"

include $(BUILD_STATIC_LIBRARY)

# Build JNI library
include $(CLEAR_VARS)

LOCAL_SRC_FILES := jni/com_intel_multidisplay_DisplaySetting.cpp

LOCAL_MODULE:= libmultidisplayjni
LOCAL_MODULE_TAGS:=optional

LOCAL_SHARED_LIBRARIES := \
     libcutils \
     libutils \
     libbinder \
     libandroid_runtime \
     libmultidisplay \
     libnativehelper

LOCAL_C_INCLUDES := \
     $(JNI_H_INCLUDE) \
     $(call include-path-for, frameworks-base)

ifeq ($(TARGET_HAS_ISV),true)
LOCAL_CFLAGS += -DTARGET_HAS_ISV
endif
LOCAL_CFLAGS += -DLOG_TAG=\"MultiDisplay\"

include $(BUILD_SHARED_LIBRARY)

# ============================================================
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
     java/com/intel/multidisplay/DisplaySetting.java \
     java/com/intel/multidisplay/DisplayObserver.java

LOCAL_MODULE:= com.intel.multidisplay
LOCAL_MODULE_TAGS:=optional

LOCAL_JNI_SHARED_LIBRARIES := libmultidisplayjni

LOCAL_NO_EMMA_INSTRUMENT := true
LOCAL_NO_EMMA_COMPILE := true

include $(BUILD_JAVA_LIBRARY)

else
# ============================================================
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
     dummy/DisplayObserver.java

LOCAL_MODULE := com.intel.multidisplay
LOCAL_MODULE_TAGS := optional

LOCAL_NO_EMMA_INSTRUMENT := true
LOCAL_NO_EMMA_COMPILE := true

include $(BUILD_JAVA_LIBRARY)

endif
include $(BUILD_DROIDDOC)
# ===========================================================
# Declare the library to the framework by copying it to /system/etc/permissions directory.
include $(CLEAR_VARS)

LOCAL_MODULE := com.intel.multidisplay.xml

LOCAL_MODULE_TAGS := optional

LOCAL_MODULE_CLASS := ETC

# This will install the file in /system/etc/permissions
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/permissions

LOCAL_SRC_FILES := $(LOCAL_MODULE)

include $(BUILD_PREBUILT)
endif
