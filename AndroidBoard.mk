LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

ALL_PREBUILT += $(INSTALLED_KERNEL_TARGET)

# include x86 encoder (apache-harmony (intel))
#include $(TOP)/dalvik/vm/compiler/codegen/x86/libenc/Android.mk

# include the non-open-source counterpart to this file
-include vendor/asus/a500cg/AndroidBoardVendor.mk
