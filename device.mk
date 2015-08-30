#
# Copyright (C) 2013 The Android Open-Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

LOCAL_PATH := device/asus/a500cg

$(call inherit-product, device/asus/a500cg/intel-boot-tools/Android.mk)

$(call inherit-product, $(SRC_TARGET_DIR)/product/languages_full.mk)

ifeq ($(TARGET_PREBUILT_KERNEL),)
	LOCAL_KERNEL := $(LOCAL_PATH)/blobs/bzImage
else
	LOCAL_KERNEL := $(TARGET_PREBUILT_KERNEL)
endif

# This device is xhdpi.  However the platform doesn't
# currently contain all of the bitmaps at xhdpi density so
# we do this little trick to fall back to the hdpi version
# if the xhdpi doesn't exist.
PRODUCT_AAPT_CONFIG := normal hdpi xhdpi
PRODUCT_AAPT_PREF_CONFIG := xhdpi

DEVICE_BASE_BOOT_IMAGE := $(LOCAL_PATH)/blobs/boot.img
DEVICE_BASE_RECOVERY_IMAGE := $(LOCAL_PATH)/blobs/recovery-ww-2.20.40.13.img
BOARD_CUSTOM_BOOTIMG_MK := $(LOCAL_PATH)/intel-boot-tools/boot.mk

# specific management of audio_policy.conf
PRODUCT_COPY_FILES += \
    device/asus/a500cg/configs/media_codecs.xml:system/etc/media_codecs.xml \
    device/asus/a500cg/audio_policy.conf:system/etc/audio_policy.conf

# Intel Display
PRODUCT_PROPERTY_OVERRIDES += \
    ro.sf.lcd_density=320

PRODUCT_COPY_FILES += \
    $(LOCAL_KERNEL):kernel

# Modules (currently from ASUS)
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/ramdisk/lib/modules/apwr3_1.ko:root/lib/modules/apwr3_1.ko \
    $(LOCAL_PATH)/ramdisk/lib/modules/atomisp-css2300.ko:root/lib/modules/atomisp-css2300.ko \
    $(LOCAL_PATH)/ramdisk/lib/modules/atomisp-css2400b0_v21.ko:root/lib/modules/atomisp-css2400b0_v21.ko \
    $(LOCAL_PATH)/ramdisk/lib/modules/atomisp-css2401a0_legacy_v21.ko:root/lib/modules/atomisp-css2401a0_legacy_v21.ko \
    $(LOCAL_PATH)/ramdisk/lib/modules/atomisp-css2401a0_v21.ko:root/lib/modules/atomisp-css2401a0_v21.ko \
    $(LOCAL_PATH)/ramdisk/lib/modules/bcm43362.ko:root/lib/modules/bcm43362.ko \
    $(LOCAL_PATH)/ramdisk/lib/modules/bcm_bt_lpm.ko:root/lib/modules/bcm_bt_lpm.ko \
    $(LOCAL_PATH)/ramdisk/lib/modules/gc2155.ko:root/lib/modules/gc2155.ko \
    $(LOCAL_PATH)/ramdisk/lib/modules/hm2056.ko:root/lib/modules/hm2056.ko \
    $(LOCAL_PATH)/ramdisk/lib/modules/hm2056_raw.ko:root/lib/modules/hm2056_raw.ko \
    $(LOCAL_PATH)/ramdisk/lib/modules/imx111.ko:root/lib/modules/imx111.ko \
    $(LOCAL_PATH)/ramdisk/lib/modules/imx219.ko:root/lib/modules/imx219.ko \
    $(LOCAL_PATH)/ramdisk/lib/modules/lm3554.ko:root/lib/modules/lm3554.ko \
    $(LOCAL_PATH)/ramdisk/lib/modules/mac80211.ko:root/lib/modules/mac80211.ko \
    $(LOCAL_PATH)/ramdisk/lib/modules/mn34130.ko:root/lib/modules/mn34130.ko \
    $(LOCAL_PATH)/ramdisk/lib/modules/modules.alias:root/lib/modules/modules.alias \
    $(LOCAL_PATH)/ramdisk/lib/modules/modules.alias.bin:root/lib/modules/modules.alias.bin \
    $(LOCAL_PATH)/ramdisk/lib/modules/modules.builtin.bin:root/lib/modules/modules.builtin.bin \
    $(LOCAL_PATH)/ramdisk/lib/modules/modules.dep:root/lib/modules/modules.dep \
    $(LOCAL_PATH)/ramdisk/lib/modules/modules.dep.bin:root/lib/modules/modules.dep.bin \
    $(LOCAL_PATH)/ramdisk/lib/modules/modules.devname:root/lib/modules/modules.devname \
    $(LOCAL_PATH)/ramdisk/lib/modules/modules.softdep:root/lib/modules/modules.softdep \
    $(LOCAL_PATH)/ramdisk/lib/modules/modules.symbols:root/lib/modules/modules.symbols \
    $(LOCAL_PATH)/ramdisk/lib/modules/modules.symbols.bin:root/lib/modules/modules.symbols.bin \
    $(LOCAL_PATH)/ramdisk/lib/modules/mt9m114.ko:root/lib/modules/mt9m114.ko \
    $(LOCAL_PATH)/ramdisk/lib/modules/ov8830.ko:root/lib/modules/ov8830.ko \
    $(LOCAL_PATH)/ramdisk/lib/modules/pax.ko:root/lib/modules/pax.ko \
    $(LOCAL_PATH)/ramdisk/lib/modules/pnwdisp.ko:root/lib/modules/pnwdisp.ko \
    $(LOCAL_PATH)/ramdisk/lib/modules/pvrsgx.ko:root/lib/modules/pvrsgx.ko \
    $(LOCAL_PATH)/ramdisk/lib/modules/rmi4.ko:root/lib/modules/rmi4.ko \
    $(LOCAL_PATH)/ramdisk/lib/modules/rt8515.ko:root/lib/modules/rt8515.ko \
    $(LOCAL_PATH)/ramdisk/lib/modules/sep3_15.ko:root/lib/modules/sep3_15.ko \
    $(LOCAL_PATH)/ramdisk/lib/modules/st_drv.ko:root/lib/modules/st_drv.ko \
    $(LOCAL_PATH)/ramdisk/lib/modules/t4k35_180.ko:root/lib/modules/t4k35_180.ko \
    $(LOCAL_PATH)/ramdisk/lib/modules/t4k35.ko:root/lib/modules/t4k35.ko \
    $(LOCAL_PATH)/ramdisk/lib/modules/t4k37.ko:root/lib/modules/t4k37.ko \
    $(LOCAL_PATH)/ramdisk/lib/modules/test_nx.ko:root/lib/modules/test_nx.ko \
    $(LOCAL_PATH)/ramdisk/lib/modules/texfat.ko:root/lib/modules/texfat.ko \
    $(LOCAL_PATH)/ramdisk/lib/modules/tntfs.ko:root/lib/modules/tntfs.ko \
    $(LOCAL_PATH)/ramdisk/lib/modules/videobuf-core.ko:root/lib/modules/videobuf-core.ko \
    $(LOCAL_PATH)/ramdisk/lib/modules/videobuf-vmalloc.ko:root/lib/modules/videobuf-vmalloc.ko \
    $(LOCAL_PATH)/ramdisk/lib/modules/vtsspp.ko:root/lib/modules/vtsspp.ko \
    $(LOCAL_PATH)/ramdisk/lib/prebuild_modules/texfat.ko:root/lib/prebuild_modules/texfat.ko \
    $(LOCAL_PATH)/ramdisk/lib/prebuild_modules/tntfs.ko:root/lib/prebuild_modules/tntfs.ko

#GPS FIX
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/configs/gpsd:system/bin/gpsd 

PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/ramdisk/sbin/intel_prop:root/sbin/intel_prop \
    $(LOCAL_PATH)/ramdisk/sbin/thermald:root/sbin/thermald \
    $(LOCAL_PATH)/ramdisk/charger:root/charger

PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/ramdisk/sbin/adbd:root/sbin/adbd \
    $(LOCAL_PATH)/ramdisk/sbin/healthd:root/sbin/healthd

PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/ramdisk/DSDS_A500CG/platform/init.props:root/local_cfg/platform/init.props \
    $(LOCAL_PATH)/ramdisk/DSDS_A500CG/telephony_config/init.props:root/local_cfg/telephony_config/init.props

# Binary to be replaced with source code ..
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/ramdisk/sbin/healthd:recovery/root/sbin/healthd \
    $(LOCAL_PATH)/ramdisk/init.recovery.redhookbay.rc:root/init.recovery.redhookbay.rc 


# Ramdisk fstab / rc files
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/ramdisk/config.sh:root/config.sh \
    $(LOCAL_PATH)/ramdisk/config_init.sh:root/config_init.sh \
    $(LOCAL_PATH)/ramdisk/fstab:root/fstab \
    $(LOCAL_PATH)/ramdisk/fstab.charger.redhookbay:root/fstab.charger \
    $(LOCAL_PATH)/ramdisk/fstab.ramconsole.redhookbay:root/fstab.ramconsole.redhookbay \
    $(LOCAL_PATH)/ramdisk/fstab.redhookbay:root/fstab.redhookbay \
    $(LOCAL_PATH)/ramdisk/fstab.zram:root/fstab.zram \
    $(LOCAL_PATH)/ramdisk/init.a500cg.usb.rc:root/init.a500cg.usb.rc \
    $(LOCAL_PATH)/ramdisk/init.avc.rc:root/init.avc.rc \
    $(LOCAL_PATH)/ramdisk/init.bt.rc:root/init.bt.rc \
    $(LOCAL_PATH)/ramdisk/init.compass.rc:root/init.compass.rc \
    $(LOCAL_PATH)/ramdisk/init.debug.rc:root/init.debug.rc \
    $(LOCAL_PATH)/ramdisk/init.diag.rc:root/init.diag.rc \
    $(LOCAL_PATH)/ramdisk/init.logtool.rc:root/init.logtool.rc \
    $(LOCAL_PATH)/ramdisk/init.bt.vendor.rc:root/init.bt.vendor.rc \
    $(LOCAL_PATH)/ramdisk/init.modem.rc:root/init.modem.rc \
    $(LOCAL_PATH)/ramdisk/init.common.rc:root/init.common.rc \
    $(LOCAL_PATH)/ramdisk/init.gps.rc:root/init.gps.rc \
    $(LOCAL_PATH)/ramdisk/init.ksm.rc:root/init.ksm.rc \
    $(LOCAL_PATH)/ramdisk/init.config_init.rc:root/init.config_init.rc \
    $(LOCAL_PATH)/ramdisk/intel_prop.cfg:root/intel_prop.cfg \
    $(LOCAL_PATH)/ramdisk/init.redhookbay.rc:root/init.redhookbay.rc \
    $(LOCAL_PATH)/ramdisk/init.wifi.rc:root/init.wifi.rc \
    $(LOCAL_PATH)/ramdisk/init.wifi.vendor.rc:root/init.wifi.vendor.rc \
    $(LOCAL_PATH)/ramdisk/init.wireless.rc:root/init.wireless.rc \
    $(LOCAL_PATH)/ramdisk/rfkill_bt.sh:root/rfkill_bt.sh \
    $(LOCAL_PATH)/ramdisk/init.watchdog.rc:root/init.watchdog.rc \
    $(LOCAL_PATH)/ramdisk/init.zram.rc:root/init.zram.rc \
    $(LOCAL_PATH)/ramdisk/init.zygote32.rc:root/init.zygote32.rc \
    $(LOCAL_PATH)/ramdisk/ueventd.redhookbay.rc:root/ueventd.redhookbay.rc \
    $(LOCAL_PATH)/ramdisk/init.aosp.rc:root/init.aosp.rc \
    $(LOCAL_PATH)/ramdisk/init.rc:root/init.rc

# Wifi
PRODUCT_PACKAGES += \
    libwpa_client \
    hostapd \
    dhcpcd.conf \
    wpa_supplicant

# Audio
PRODUCT_PACKAGES += \
    libtinycompress \
    libtinyalsa \
    audio.a2dp.default \
    audio.primary.default \
    audio.r_submix.default \
    audio.usb.default \
    libtinyalsa-subsystem \
    libaudioutils

# stagefright
PRODUCT_PACKAGES += \
    libstagefrighthw

# omx common
PRODUCT_PACKAGES += \
    libwrs_omxil_common \
    libwrs_omxil_core_pvwrapped

# video decoder encoder
PRODUCT_PACKAGES += \
    libOMXVideoDecoderAVC \
    libOMXVideoDecoderH263 \
    libOMXVideoDecoderMPEG4 \
    libOMXVideoDecoderWMV \
    libOMXVideoEncoderAVC \
    libOMXVideoEncoderH263 \
    libOMXVideoEncoderMPEG4 \
    libOMXVideoDecoderAVCSecure

# libwsbm
PRODUCT_PACKAGES += \
    libwsbm

# libmix
PRODUCT_PACKAGES += \
    libmixvbp \
    libmixvbp_h264 \
    libmixvbp_h264secure \
    libmixvbp_mpeg4 \
    libmixvbp_vc1

# image decover
PRODUCT_PACKAGES += \
    libmix_imagedecoder \
    libmix_imageencoder

# Media SDK and OMX IL components
PRODUCT_PACKAGES += \
    msvdx_bin \
    topaz_bin

PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/handheld_core_hardware.xml:system/etc/permissions/handheld_core_hardware.xml \
    frameworks/native/data/etc/android.hardware.camera.autofocus.xml:system/etc/permissions/android.hardware.camera.autofocus.xml \
    frameworks/native/data/etc/android.hardware.camera.flash-autofocus.xml:system/etc/permissions/android.hardware.camera.flash-autofocus.xml \
    frameworks/native/data/etc/android.hardware.camera.front.xml:system/etc/permissions/android.hardware.camera.front.xml \
    frameworks/native/data/etc/android.hardware.camera.xml:system/etc/permissions/android.hardware.camera.xml \
    frameworks/native/data/etc/android.hardware.location.xml:system/etc/permissions/android.hardware.location.xml \
    frameworks/native/data/etc/android.hardware.location.gps.xml:system/etc/permissions/android.hardware.location.gps.xml \
    frameworks/native/data/etc/android.hardware.wifi.xml:system/etc/permissions/android.hardware.wifi.xml \
    frameworks/native/data/etc/android.hardware.sensor.accelerometer.xml:system/etc/permissions/android.hardware.sensor.accelerometer.xml \
    frameworks/native/data/etc/android.hardware.sensor.gyroscope.xml:system/etc/permissions/android.hardware.sensor.gyroscope.xml \
    frameworks/native/data/etc/android.hardware.sensor.compass.xml:system/etc/permissions/android.hardware.sensor.compass.xml \
    frameworks/native/data/etc/android.hardware.sensor.light.xml:system/etc/permissions/android.hardware.sensor.light.xml \
    frameworks/native/data/etc/android.hardware.sensor.proximity.xml:system/etc/permissions/android.hardware.sensor.proximity.xml \
    frameworks/native/data/etc/android.hardware.telephony.gsm.xml:system/etc/permissions/android.hardware.telephony.gsm.xml \
    frameworks/native/data/etc/android.hardware.touchscreen.multitouch.distinct.xml:system/etc/permissions/android.hardware.touchscreen.multitouch.distinct.xml \
    frameworks/native/data/etc/android.hardware.touchscreen.multitouch.jazzhand.xml:system/etc/permissions/android.hardware.touchscreen.multitouch.jazzhand.xml \
    frameworks/native/data/etc/android.hardware.touchscreen.multitouch.xml:system/etc/permissions/android.hardware.touchscreen.multitouch.xml \
    frameworks/native/data/etc/android.hardware.usb.accessory.xml:system/etc/permissions/android.hardware.usb.accessory.xml \
    frameworks/native/data/etc/android.hardware.touchscreen.xml:system/etc/permissions/android.hardware.touchscreen.xml \
    frameworks/native/data/etc/android.hardware.usb.host.xml:system/etc/permissions/android.hardware.usb.host.xml \
    frameworks/native/data/etc/android.hardware.wifi.direct.xml:system/etc/permissions/android.hardware.wifi.direct.xml \
    frameworks/native/data/etc/android.hardware.wifi.xml:system/etc/permissions/android.hardware.wifi.xml \
    frameworks/native/data/etc/android.software.sip.voip.xml:system/etc/permissions/android.software.sip.voip.xml \
    frameworks/native/data/etc/android.software.webview.xml:system/etc/permissions/android.software.webview.xml \
    frameworks/native/data/etc/android.hardware.bluetooth_le.xml:system/etc/permissions/android.hardware.bluetooth_le.xml

PRODUCT_PROPERTY_OVERRIDES += \
    ro.sf.lcd_density=320 \
    ro.opengles.version = 196608

PRODUCT_PACKAGES += \
    audio.a2dp.default \
    audio.usb.default \
    audio.r_submix.default \
    audio.usb.default \
    audio.primary.default

# usb
PRODUCT_PACKAGES += \
    com.android.future.usb.accessory

# Filesystem management tools
PRODUCT_PACKAGES += \
    e2fsck \
    tune2fs \
    resize2fs

PRODUCT_PACKAGES += \
    Stk

# library 
PRODUCT_PACKAGES += \
    libtinyxml \
    minizip \
    openssl \
    pack_intel \
    unpack_intel

PRODUCT_PACKAGES += \
    link_modprobe

PRODUCT_PACKAGES += \
    su \
    com.intel.multidisplay.xml \
    com.intel.multidisplay \
    libmultidisplay \
    libmultidisplayjni

DEVICE_PACKAGE_OVERLAYS := \
    $(LOCAL_PATH)/overlay

#ituxd for thermal management
ENABLE_ITUXD := true
PRODUCT_PACKAGES += \
        ituxd

# This will build the plugins/libart-extension.so library
PRODUCT_PACKAGES += libart-extension libartd-extension

ADDITIONAL_DEFAULT_PROPERTIES += \
    ro.debuggable=1 \
    ro.secure=0 \
    ro.adb.secure=0 

PRODUCT_PROPERTY_OVERRIDES += \
    persist.sys.root_access=3 \
    ro.dalvik.vm.isa.arm=x86 \
    ro.enable.native.bridge.exec=1

# call dalvik heap config
$(call inherit-product-if-exists, frameworks/native/build/phone-xhdpi-1024-dalvik-heap.mk)

# call hwui memory config
#$(call inherit-product-if-exists, frameworks/native/build/phone-xxhdpi-2048-hwui-memory.mk)
