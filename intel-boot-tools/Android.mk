LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := pack.c
LOCAL_CFLAGS += -I/usr/include/
LOCAL_MODULE := pack_intel

include $(BUILD_HOST_EXECUTABLE)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := unpack.c
LOCAL_CFLAGS += -I/usr/include/
LOCAL_MODULE := unpack_intel

include $(BUILD_HOST_EXECUTABLE)
$(call dist-for-goals,dist_files,$(LOCAL_BUILT_MODULE))
