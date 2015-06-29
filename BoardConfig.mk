include $(GENERIC_X86_CONFIG_MK)
TARGET_SPECIFIC_HEADER_PATH := $(LOCAL_PATH)/include
LOCAL_PATH := device/asus/a500cg
BOARD_CREATE_MODPROBE_SYMLINK := true

TARGET_NO_BOOTLOADER := true
TARGET_NO_RADIOIMAGE := true

TARGET_ARCH := x86
#TARGET_ARCH_VARIANT := x86
TARGET_ARCH_VARIANT := x86-atom
#TARGET_ARCH_VARIANT := silvermont
TARGET_CPU_ABI := x86
TARGET_CPU_ABI2 := armeabi-v7a
TARGET_CPU_ABI_LIST := x86,armeabi-v7a,armeabi
TARGET_CPU_ABI_LIST_32_BIT := x86,armeabi-v7a,armeabi
TARGET_CPU_SMP := true
TARGET_RELEASETOOLS_EXTENSIONS := device/asus/a500cg/releasetools
BUILD_EMULATOR := false
INTEL_INGREDIENTS_VERSIONS := true
LOCAL_CFLAGS += -DARCH_IA32
TARGET_PRELINK_MODULE := false
#ART_USE PORTABLE COMPILER


# Atom optimizations to improve memory benchmarks.
-include $(LOCAL_PATH)/OptAtom.mk

TARGET_RECOVERY_FSTAB := $(LOCAL_PATH)/ramdisk/fstab.redhookbay

TARGET_BOARD_PLATFORM := clovertrail
TARGET_BOOTLOADER_BOARD_NAME := clovertrail
TARGET_USERIMAGES_USE_EXT4 := true
BOARD_SYSTEMIMAGE_PARTITION_SIZE := 1363148800
BOARD_FLASH_BLOCK_SIZE := 131072


TARGET_RECOVERY_PIXEL_FORMAT := "BGRA_8888"


# Use dlmalloc
MALLOC_IMPL := dlmalloc


# skip doc from building
BOARD_SKIP_ANDROID_DOC_BUILD := true

BOARD_MALLOC_ALIGNMENT := 16

# Appends path to ARM libs for Houdini
PRODUCT_LIBRARY_PATH := $(PRODUCT_LIBRARY_PATH):/system/lib/arm

# Inline kernel building
#TARGET_KERNEL_CONFIG := i386_ctp_defconfig
#TARGET_KERNEL_SOURCE := linux/kernel
#TARGET_KERNEL_ARCH := x86_64
TARGET_PREBUILT_RECOVERY_KERNEL := $(LOCAL_PATH)/blobs/bzImage


# Kernel config (reference only)


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
ifeq ($(TARGET_BUILD_VARIANT),user)
    ifeq ($(WITH_DEXPREOPT),)
      WITH_DEXPREOPT := true
    endif
endif

# NFC
BOARD_HAVE_NFC := false

# Security
#BUILD_WITH_SECURITY_FRAMEWORK := chaabi_token
#BUILD_WITH_CHAABI_SUPPORT := true


# Wifi
BOARD_WLAN_DEVICE := bcmdhd
BOARD_WPA_SUPPLICANT_PRIVATE_LIB := lib_driver_cmd_bcmdhd
BOARD_HOSTAPD_PRIVATE_LIB := lib_driver_cmd_bcmdhd
WPA_SUPPLICANT_VERSION := VER_0_8_X
BOARD_WPA_SUPPLICANT_DRIVER := NL80211
BOARD_HOSTAPD_DRIVER := NL80211
WIFI_DRIVER_FW_PATH_PARAM := "/sys/module/bcm43362/parameters/firmware_path"
WIFI_DRIVER_FW_PATH_AP    := "/system/etc/firmware/fw_bcmdhd_43362_apsta.bin"
WIFI_DRIVER_FW_PATH_STA   := "/system/etc/firmware/fw_bcmdhd_43362.bin"
WIFI_DRIVER_MODULE_ARG := "iface_name=wlan0 firmware_path=/system/etc/firmware/fw_bcmdhd_43362.bin"


# Binder API version
TARGET_USES_64_BIT_BINDER := true

#BINDER_IPC_32BIT := true
#HOST_PREFER_32_BIT := true
#TARGET_PREFER_32_BIT := true
#LOCAL_32_BIT_ONLY := true
#TARGET_SUPPORTS_32_BIT_APPS := true


# Bluetooth
BOARD_HAVE_BLUETOOTH := true
BOARD_HAVE_BLUETOOTH_BCM := true

# IMG graphics
#BOARD_GFX_REV := RGX6400
#ENABLE_IMG_GRAPHICS := true
#ENABLE_MRFL_GRAPHICS := true
#INTEL_HWC_MOOREFIELD := true
#HWUI_IMG_FBO_CACHE_OPTIM := true
#TARGET_SUPPORT_HDMI_PRIMARY := true



# Include an expanded selection of fonts
EXTENDED_FONT_FOOTPRINT := true

#PRODUCT_DEFAULT_PROPERTY_OVERRIDES += ro.zygote=zygote32

# Audio
BOARD_USES_ALSA_AUDIO := true
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

# Include an expanded selection of fonts
# EXTENDED_FONT_FOOTPRINT := true


ADDITIONAL_DEFAULT_PROPERTIES += \
    ro.sf.lcd_density=320 \
    ro.opengles.version = 196608 \
    gsm.net.interface=rmnet0 \
    persist.system.at-proxy.mode=0 \
    ro.dalvik.vm.native.bridge=libhoudini.so \
    dalvik.vm.dex2oat-Xms=64m \
    dalvik.vm.dex2oat-Xmx=512m \
    dalvik.vm.image-dex2oat-Xms=64m \
    dalvik.vm.image-dex2oat-Xmx=64m


# File system
# Storage information
BOARD_VOLD_EMMC_SHARES_DEV_MAJOR := true
BOARD_VOLD_DISC_HAS_MULTIPLE_MAJORS := true
BOARD_HAS_LARGE_FILESYSTEM := true



# Recovery global
TARGET_RECOVERY_INITRC := $(LOCAL_PATH)/ramdisk/recovery.init.redhookbay.rc
BOARD_RECOVERY_SWIPE := true
BOARD_UMS_LUNFILE := "/sys/devices/virtual/android_usb/android0/f_mass_storage/lun/file"

# TWR
# Recovery options TWRP
DEVICE_RESOLUTION := 720x1280
TW_INCLUDE_CRYPTO := true
RECOVERY_GRAPHICS_USE_LINELENGTH := true
BOARD_USE_CUSTOM_RECOVERY_FONT := \"roboto_15x24.h\"
TWRP_EVENT_LOGGING := false

TW_INTERNAL_STORAGE_PATH := "/data/media"
TW_INTERNAL_STORAGE_MOUNT_POINT := "data"
TW_EXTERNAL_STORAGE_PATH := "/MicroSD"
TW_EXTERNAL_STORAGE_MOUNT_POINT := "MicroSD"
TW_DEFAULT_EXTERNAL_STORAGE := true

TW_CUSTOM_BATTERY_PATH := /sys/class/power_supply/max170xx_battery
TW_BRIGHTNESS_PATH := /sys/class/backlight/psb-bl/brightness
TW_NO_SCREEN_BLANK := true
TW_MAX_BRIGHTESS := 255


#HAVE_SELINUX := true

# SELinux
HAVE_SELINUX := true
BOARD_SEPOLICY_DIRS += device/asus/a500cg/sepolicy

BOARD_SEPOLICY_UNION += \
    file_contexts \
    seapp_contexts \
    property_contexts \
    service_contexts \
    file.te \
    device.te \
    ecryptfs.te \
    genfs_contexts \
    vold.te \
    surfaceflinger.te \
    zygote.te \
    pvrsrvctl.te \
    bluetooth.te \
    surfaceflinger.te \
    system_app.te \
    file.te \
    shell.te \
    mediaserver.te \
    nvm_server.te \
    su.te   \
    system_server.te \
    service.te \
    mmgr.te \
    init.te \
    kernel.te \
    sysfs_uart_power_ctrl.te \
    ueventd.te \
    logcat.te \
    netd.te \
    wpa.te \
    rild.te \
    akmd.te \
    untrusted_app.te

#BOARD_SEPOLICY_UNION += \
#	sepfs.te 
#SOUND_TRIGGER_USE_STUB_MODULE:=true
USE_INTEL_MDP := true
BUILD_WITH_FULL_STAGEFRIGHT := true
BOARD_USES_WRS_OMXIL_CORE := true
BOARD_USE_LIBVA_INTEL_DRIVER := true
BOARD_USE_LIBVA := true
BOARD_USE_LIBMIX := true
#INTEL_VA := true
