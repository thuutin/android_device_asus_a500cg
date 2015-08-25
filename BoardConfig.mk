include $(GENERIC_X86_CONFIG_MK)
LOCAL_PATH := device/asus/a500cg
TARGET_SPECIFIC_HEADER_PATH := $(LOCAL_PATH)/include

BOARD_CREATE_MODPROBE_SYMLINK := true

TARGET_NO_BOOTLOADER := true
TARGET_NO_RADIOIMAGE := true
TARGET_NO_RECOVERY := false
TARGET_ARCH := x86
TARGET_ARCH_VARIANT := x86-atom
TARGET_CPU_ABI := x86
TARGET_CPU_VARIANT := x86
TARGET_CPU_ABI2 := armeabi-v7a
TARGET_CPU_ABI_LIST := x86,armeabi-v7a,armeabi
TARGET_CPU_ABI_LIST_32_BIT := x86,armeabi-v7a,armeabi
TARGET_CPU_SMP := true
#SIM_COUNT := 2
#ANDROID_MULTI_SIM := true
TARGET_RIL_DISABLE_STATUS_POLLING := true

TARGET_BOARD_KERNEL_HEADERS := device/asus/a500cg/kernel-headers 

# Bootloader
TARGET_OTA_ASSERT_DEVICE := a500cg,ASUS_T00F

INTEL_INGREDIENTS_VERSIONS := true
LOCAL_CFLAGS += -DARCH_IA32
TARGET_PRELINK_MODULE := false

#add some intel BOOTCLASSPATH
#PRODUCT_BOOT_JARS += com.intel.config com.intel.multidisplay
PRODUCT_BOOT_JARS += com.intel.multidisplay


# skip some proccess to speed up build
BOARD_SKIP_ANDROID_DOC_BUILD := true
BUILD_EMULATOR := false

# enable ARM codegen for x86 with Houdini
BUILD_ARM_FOR_X86 := true

TARGET_RECOVERY_FSTAB := device/asus/a500cg/ramdisk/fstab.redhookbay

TARGET_BOARD_PLATFORM := clovertrail
TARGET_BOOTLOADER_BOARD_NAME := clovertrail
TARGET_USERIMAGES_USE_EXT4 := true
BOARD_SYSTEMIMAGE_PARTITION_SIZE := 1363148800
BOARD_FLASH_BLOCK_SIZE := 131072

TARGET_RECOVERY_PIXEL_FORMAT := "BGRA_8888"

# Use dlmalloc
MALLOC_IMPL := dlmalloc

# Malloc Alignment
BOARD_MALLOC_ALIGNMENT := 16

# Appends path to ARM libs for Houdini
PRODUCT_LIBRARY_PATH := $(PRODUCT_LIBRARY_PATH):/system/lib/arm

# Kernel Build from source inline
TARGET_PROVIDES_INIT_RC := true
#MINIGZIP := out/host/darwin-x86/bin/minigzip
#openssl: out/host/darwin-x86/bin/openssl
#KERNEL_DIFFCONFIG := $(LOCAL_PATH)/a500cg_defconfig

#$(info KERNEL_DIFFCONFIG ${KERNEL_DIFFCONFIG} )
#KERNEL_MODULES_INSTALL := root
#TARGET_KERNEL_BUILT_FROM_SOURCE := true
TARGET_KERNEL_CROSS_COMPILE_PREFIX := x86_64-linux-android-
#TARGET_KERNEL_CONFIG := a500cg_defconfig
#TARGET_KERNEL_VARIANT_CONFIG := asusctp_hd_diffconfig
TARGET_KERNEL_SOURCE := kernel/asus/a500cg
TARGET_KERNEL_ARCH := x86

# BOARD_CUSTOM_BOOTIMG_MK := device/asus/a500cg/intel-boot-tools/boot.mk
# DEVICE_BASE_BOOT_IMAGE := device/asus/a500cg/blobs/boot.img
BOARD_KERNEL_IMAGE_NAME := bzImage

# prebuild source kernel
BOARD_CUSTOM_BOOTIMG_MK := device/asus/a500cg/intel-boot-tools/boot.mk
DEVICE_BASE_BOOT_IMAGE := device/asus/a500cg/blobs/boot.img
DEVICE_BASE_RECOVERY_IMAGE := device/asus/a500cg/blobs/recovery-WW-3.23.40.52.img
TARGET_PREBUILT_KERNEL := device/asus/a500cg/blobs/bzImage 

# Kernel config (reference only)
BOARD_KERNEL_BASE := 0x10000000
BOARD_KERNEL_PAGESIZE := 2048
cmdline_extra := watchdog.watchdog_thresh=60 androidboot.spid=xxxx:xxxx:xxxx:xxxx:xxxx:xxxx androidboot.serialno=01234567890123456789012345678901
cmdline_extra1 := ip=50.0.0.2:50.0.0.1::255.255.255.0::usb0:on vmalloc=172M androidboot.wakesrc=05 androidboot.mode=main loglevel=8 
cmdline_extra2 := loglevel=8 kmemleak=off androidboot.bootmedia=sdcard androidboot.hardware=redhookbay androidboot.selinux=permissive
BOARD_KERNEL_CMDLINE := init=/init pci=noearly console=logk0 earlyprintk=nologger  $(cmdline_extra)  $(cmdline_extra1)  $(cmdline_extra2) 

BOARD_EGL_CFG := $(LOCAL_PATH)/configs/egl.cfg
BOARD_EGL_WORKAROUND_BUG_10194508 := true
TARGET_RUNNING_WITHOUT_SYNC_FRAMEWORK := true

# Enable dex-preoptimization to speed up first boot sequence
ifeq ($(WITH_DEXPREOPT),)
  WITH_DEXPREOPT := true
endif

# Wifi
BOARD_WLAN_DEVICE 			:= bcmdhd
BOARD_WPA_SUPPLICANT_PRIVATE_LIB 	:= lib_driver_cmd_bcmdhd
BOARD_HOSTAPD_PRIVATE_LIB        	:= lib_driver_cmd_bcmdhd
WPA_SUPPLICANT_VERSION 			:= VER_0_8_X
BOARD_WPA_SUPPLICANT_DRIVER 		:= NL80211
BOARD_HOSTAPD_DRIVER 			:= NL80211
#WIFI_DRIVER_MODULE_PATH         	:= "/system/lib/modules/dhd.ko"
WIFI_DRIVER_FW_PATH_PARAM 		:= "/sys/module/bcm43362/parameters/firmware_path"
WIFI_DRIVER_FW_PATH_AP    		:= "/system/etc/firmware/fw_bcmdhd_43362_apsta.bin"
WIFI_DRIVER_FW_PATH_STA   		:= "/system/etc/firmware/fw_bcmdhd_43362.bin"
WIFI_DRIVER_MODULE_ARG 			:= "iface_name=wlan0 firmware_path=/system/etc/firmware/fw_bcmdhd_43362.bin"

# Bluetooth
BOARD_HAVE_BLUETOOTH := true
BOARD_HAVE_BLUETOOTH_BCM := true


PRODUCT_DEFAULT_PROPERTY_OVERRIDES += ro.zygote=zygote32

ADDITIONAL_DEFAULT_PROPERTIES += \
    ro.config.low_ram=false \
    ro.ril.status.polling.enable=0 \
    ro.product.cpu.abi2=armeabi-v7a \
    ro.config.personality=compat_layout

# NFC
BOARD_HAVE_NFC := false

#BINDER
TARGET_USES_64_BIT_BINDER := true

# Audio
BOARD_USES_TINY_ALSA_AUDIO := true

# DRM Protected Video
BOARD_WIDEVINE_OEMCRYPTO_LEVEL := 1
USE_INTEL_SECURE_AVC := true

# enable ARM codegen for x86 with Houdini
BUILD_ARM_FOR_X86 := true

# HW_Renderer
USE_OPENGL_RENDERER := true

# DPST
INTEL_DPST := true

# HWComposer
BOARD_USES_HWCOMPOSER := true

# RILD
#RIL_SUPPORTS_SEEK := true

# GPS
BOARD_HAVE_GPS := true

# RMT_STORAGE
BOARD_USES_LEGACY_MMAP := true

ADDITIONAL_DEFAULT_PROPERTIES += \
    ro.sf.lcd_density=320 \
    ro.opengles.version=196608 \
    gsm.net.interface=rmnet0 \
    persist.system.at-proxy.mode=0 \
    ro.dalvik.vm.native.bridge=libhoudini.so \
    dalvik.vm.dex2oat-Xms=64m \
    dalvik.vm.dex2oat-Xmx=512m \
    dalvik.vm.image-dex2oat-Xms=64m \
    dalvik.vm.image-dex2oat-Xmx=64m

# Storage information
BOARD_HAS_LARGE_FILESYSTEM := true

# Recovery global
TARGET_RECOVERY_INITRC := $(LOCAL_PATH)/ramdisk/init.recovery.redhookbay.rc
BOARD_RECOVERY_SWIPE := true
BOARD_UMS_LUNFILE := "/sys/devices/virtual/android_usb/android0/f_mass_storage/lun/file"

# TWRP Recovery
# update your twrp.fstab to recovery dir /etc/twrp.fstab
TW_NO_LEGACY_PROPS := true
RECOVERY_GRAPHICS_USE_LINELENGTH := true
TW_DISABLE_DOUBLE_BUFFERING_CHECK := true
TW_THEME := portrait_hdpi

TW_BRIGHTNESS_PATH := /sys/class/backlight/psb-bl/brightness
BOARD_HAS_NO_SELECT_BUTTON := true
BOARD_USE_CUSTOM_RECOVERY_FONT := \"roboto_15x24.h\"
TW_INCLUDE_CRYPTO := true
TW_INTERNAL_STORAGE_PATH := "/media/data/0"
TW_INTERNAL_STORAGE_MOUNT_POINT := "/data"
TW_EXTERNAL_STORAGE_PATH := "/external_sd"
TW_EXTERNAL_STORAGE_MOUNT_POINT := "external_sd"
TW_DEFAULT_EXTERNAL_STORAGE := true
TW_HAS_MTP := true
TW_NO_SCREEN_BLANK := true
TW_NO_SCREEN_TIMEOUT := true
TW_CUSTOM_BATTERY_PATH := /sys/class/power_supply/max170xx_battery
TW_BRIGHTNESS_PATH := /sys/class/backlight/psb-bl/brightness
TW_MAX_BRIGHTNESS := 255
TW_CUSTOM_POWER_BUTTON := 116
TARGET_USE_CUSTOM_LUN_FILE_PATH := "/sys/devices/virtual/android_usb/android0/f_mass_storage/lun/file"
TW_CUSTOM_CPU_TEMP_PATH := "/sys/devices/virtual/thermal/thermal_zone0/temp"
TWRP_EVENT_LOGGING := false



# SELinux
#HAVE_SELINUX := true
#BOARD_SEPOLICY_DIRS += device/asus/a500cg/sepolicy

#BOARD_SEPOLICY_UNION += \
#    file_contexts \
#    seapp_contexts \
#    property_contexts \
#    service_contexts \
#    file.te \
#    device.te \
#    ecryptfs.te \
#    genfs_contexts \
#    vold.te \
#    surfaceflinger.te \
#    zygote.te \
#    pvrsrvctl.te \
#    bluetooth.te \
#    surfaceflinger.te \
#    system_app.te \
#    file.te \
#    shell.te \
#    mediaserver.te \
#    nvm_server.te \
#    su.te   \
#    system_server.te \
#    service.te \
#    mmgr.te \
#    init.te \
#   kernel.te \
#    sysfs_uart_power_ctrl.te \
#    ueventd.te \
#   logcat.te \
#    netd.te \
#    wpa.te \
#    rild.te \
#    akmd.te \
#    gauge.te \
#    customize.te \
#    untrusted_app.te

# Build From source
USE_INTEL_MDP := true
BUILD_WITH_FULL_STAGEFRIGHT := true
BOARD_USES_MRST_OMX := true
BOARD_USES_WRS_OMXIL_CORE := true
BOARD_USE_LIBVA_INTEL_DRIVER := true
BOARD_USE_LIBVA := true
BOARD_USE_LIBMIX := true
INTEL_VA := true
ENABLE_IMG_GRAPHICS := true
# Enable Minikin text layout engine (will be the default soon)
#USE_MINIKIN := true
USE_MDS_LEGACY := true
TARGET_HAS_MULTIPLE_DISPLAY := true
#TARGET_HAS_ISV := true
#USE_HW_VP8 := true

# Rild
# Radio
BOARD_RIL_SUPPORTS_MULTIPLE_CLIENTS := true
#BOARD_RIL_CLASS := ../../../device/asus/a500cg/ril

#BOARD_FUNCTIONFS_HAS_SS_COUNT := true

# Init
TARGET_IGNORE_RO_BOOT_SERIALNO := true
