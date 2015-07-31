/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ***   To edit the content of this header, modify the corresponding
 ***   source file (e.g. under external/kernel-headers/original/) then
 ***   run bionic/libc/kernel/tools/update_all.py
 ***
 ***   Any manual change here will be lost the next time this script will
 ***   be run. You've been warned!
 ***
 ****************************************************************************
 ****************************************************************************/
#ifndef _PSB_DRM_H_
#define _PSB_DRM_H_
#ifdef __linux__
#include <stdbool.h>
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#include <stdint.h>
#include <linux/types.h>
#include "drm_mode.h"
#endif
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define PSB_PACKAGE_VERSION "5.6.0.1202"
#define DRM_PSB_SAREA_MAJOR 0
#define DRM_PSB_SAREA_MINOR 2
#define PSB_FIXED_SHIFT 16
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define PSB_NUM_PIPE 3
#define DRM_PSB_MEM_MMU TTM_PL_PRIV1
#define DRM_PSB_FLAG_MEM_MMU TTM_PL_FLAG_PRIV1
#define TTM_PL_CI TTM_PL_PRIV0
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define TTM_PL_FLAG_CI TTM_PL_FLAG_PRIV0
#define TTM_PL_RAR TTM_PL_PRIV2
#define TTM_PL_FLAG_RAR TTM_PL_FLAG_PRIV2
#define TTM_PL_IMR TTM_PL_PRIV2
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define TTM_PL_FLAG_IMR TTM_PL_FLAG_PRIV2
#define DRM_PSB_MEM_MMU_TILING TTM_PL_PRIV3
#define DRM_PSB_FLAG_MEM_MMU_TILING TTM_PL_FLAG_PRIV3
typedef enum {
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 DRM_CMD_SUCCESS,
 DRM_CMD_FAILED,
 DRM_CMD_HANG
} drm_cmd_status_t;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
struct drm_psb_scanout {
 uint32_t buffer_id;
 uint32_t rotation;
 uint32_t stride;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint32_t depth;
 uint32_t width;
 uint32_t height;
 int32_t transform[3][3];
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
};
#define DRM_PSB_SAREA_OWNERS 16
#define DRM_PSB_SAREA_OWNER_2D 0
#define DRM_PSB_SAREA_OWNER_3D 1
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define DRM_PSB_SAREA_SCANOUTS 3
struct drm_psb_sarea {
 uint32_t major;
 uint32_t minor;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint32_t ctx_owners[DRM_PSB_SAREA_OWNERS];
 uint32_t num_scanouts;
 struct drm_psb_scanout scanouts[DRM_PSB_SAREA_SCANOUTS];
 int planeA_x;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 int planeA_y;
 int planeA_w;
 int planeA_h;
 int planeB_x;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 int planeB_y;
 int planeB_w;
 int planeB_h;
 uint32_t num_active_scanouts;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
};
#define PSB_RELOC_MAGIC 0x67676767
#define PSB_RELOC_SHIFT_MASK 0x0000FFFF
#define PSB_RELOC_SHIFT_SHIFT 0
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define PSB_RELOC_ALSHIFT_MASK 0xFFFF0000
#define PSB_RELOC_ALSHIFT_SHIFT 16
#define PSB_RELOC_OP_OFFSET 0
struct drm_psb_reloc {
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint32_t reloc_op;
 uint32_t where;
 uint32_t buffer;
 uint32_t mask;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint32_t shift;
 uint32_t pre_add;
 uint32_t background;
 uint32_t dst_buffer;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint32_t arg0;
 uint32_t arg1;
};
#define PSB_GPU_ACCESS_READ (1ULL << 32)
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define PSB_GPU_ACCESS_WRITE (1ULL << 33)
#define PSB_GPU_ACCESS_MASK (PSB_GPU_ACCESS_READ | PSB_GPU_ACCESS_WRITE)
#define PSB_BO_FLAG_COMMAND (1ULL << 52)
#define PSB_ENGINE_2D 2
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define PSB_ENGINE_DECODE 0
#define PSB_ENGINE_VIDEO 0
#define LNC_ENGINE_ENCODE 1
#ifdef MERRIFIELD
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define PSB_NUM_ENGINES 7
#else
#define PSB_NUM_ENGINES 2
#endif
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define VSP_ENGINE_VPP 6
#define _PSB_FENCE_EXE_SHIFT 0
#define _PSB_FENCE_FEEDBACK_SHIFT 4
#define _PSB_FENCE_TYPE_EXE (1 << _PSB_FENCE_EXE_SHIFT)
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define _PSB_FENCE_TYPE_FEEDBACK (1 << _PSB_FENCE_FEEDBACK_SHIFT)
#define PSB_FEEDBACK_OP_VISTEST (1 << 0)
struct drm_psb_extension_rep {
 int32_t exists;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint32_t driver_ioctl_offset;
 uint32_t sarea_offset;
 uint32_t major;
 uint32_t minor;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint32_t pl;
};
#define DRM_PSB_EXT_NAME_LEN 128
union drm_psb_extension_arg {
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 char extension[DRM_PSB_EXT_NAME_LEN];
 struct drm_psb_extension_rep rep;
};
struct psb_validate_req {
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint64_t set_flags;
 uint64_t clear_flags;
 uint64_t next;
 uint64_t presumed_gpu_offset;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint32_t buffer_handle;
 uint32_t presumed_flags;
 uint32_t pad64;
 uint32_t unfence_flag;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
};
struct psb_validate_rep {
 uint64_t gpu_offset;
 uint32_t placement;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint32_t fence_type_mask;
};
#define PSB_USE_PRESUMED (1 << 0)
struct psb_validate_arg {
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint64_t handled;
 uint64_t ret;
 union {
 struct psb_validate_req req;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 struct psb_validate_rep rep;
 } d;
};
#define DRM_PSB_FENCE_NO_USER (1 << 0)
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
struct psb_ttm_fence_rep {
 uint32_t handle;
 uint32_t fence_class;
 uint32_t fence_type;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint32_t signaled_types;
 uint32_t error;
};
typedef struct drm_psb_cmdbuf_arg {
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint64_t buffer_list;
 uint64_t fence_arg;
 uint32_t cmdbuf_handle;
 uint32_t cmdbuf_offset;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint32_t cmdbuf_size;
 uint32_t reloc_handle;
 uint32_t reloc_offset;
 uint32_t num_relocs;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint32_t fence_flags;
 uint32_t engine;
} drm_psb_cmdbuf_arg_t;
typedef struct drm_psb_pageflip_arg {
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint32_t flip_offset;
 uint32_t stride;
} drm_psb_pageflip_arg_t;
typedef enum {
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 LNC_VIDEO_DEVICE_INFO,
 LNC_VIDEO_GETPARAM_IMR_INFO,
 LNC_VIDEO_GETPARAM_CI_INFO,
 LNC_VIDEO_FRAME_SKIP,
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 IMG_VIDEO_DECODE_STATUS,
 IMG_VIDEO_NEW_CONTEXT,
 IMG_VIDEO_RM_CONTEXT,
 IMG_VIDEO_UPDATE_CONTEXT,
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 IMG_VIDEO_MB_ERROR,
 IMG_VIDEO_SET_DISPLAYING_FRAME,
 IMG_VIDEO_GET_DISPLAYING_FRAME,
 IMG_VIDEO_GET_HDMI_STATE,
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 IMG_VIDEO_SET_HDMI_STATE,
 PNW_VIDEO_QUERY_ENTRY,
 IMG_DISPLAY_SET_WIDI_EXT_STATE,
 IMG_VIDEO_IED_STATE
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
} lnc_getparam_key_t;
struct drm_lnc_video_getparam_arg {
 uint64_t key;
 uint64_t arg;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint64_t value;
};
struct drm_video_displaying_frameinfo {
 uint32_t buf_handle;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint32_t width;
 uint32_t height;
 uint32_t size;
 uint32_t format;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint32_t luma_stride;
 uint32_t chroma_u_stride;
 uint32_t chroma_v_stride;
 uint32_t luma_offset;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint32_t chroma_u_offset;
 uint32_t chroma_v_offset;
 uint32_t reserved;
};
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
struct drm_psb_vistest {
 uint32_t vt[8];
};
struct drm_psb_sizes_arg {
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint32_t ta_mem_size;
 uint32_t mmu_size;
 uint32_t pds_size;
 uint32_t rastgeom_size;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint32_t tt_size;
 uint32_t vram_size;
};
struct drm_psb_hist_status_arg {
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint32_t buf[32];
};
struct drm_psb_dpst_lut_arg {
 uint8_t lut[256];
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 int output_id;
};
struct mrst_timing_info {
 uint16_t pixel_clock;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint8_t hactive_lo;
 uint8_t hblank_lo;
 uint8_t hblank_hi:4;
 uint8_t hactive_hi:4;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint8_t vactive_lo;
 uint8_t vblank_lo;
 uint8_t vblank_hi:4;
 uint8_t vactive_hi:4;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint8_t hsync_offset_lo;
 uint8_t hsync_pulse_width_lo;
 uint8_t vsync_pulse_width_lo:4;
 uint8_t vsync_offset_lo:4;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint8_t vsync_pulse_width_hi:2;
 uint8_t vsync_offset_hi:2;
 uint8_t hsync_pulse_width_hi:2;
 uint8_t hsync_offset_hi:2;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint8_t width_mm_lo;
 uint8_t height_mm_lo;
 uint8_t height_mm_hi:4;
 uint8_t width_mm_hi:4;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint8_t hborder;
 uint8_t vborder;
 uint8_t unknown0:1;
 uint8_t hsync_positive:1;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint8_t vsync_positive:1;
 uint8_t separate_sync:2;
 uint8_t stereo:1;
 uint8_t unknown6:1;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint8_t interlaced:1;
} __attribute__((packed));
struct gct_r10_timing_info {
 uint16_t pixel_clock;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint32_t hactive_lo:8;
 uint32_t hactive_hi:4;
 uint32_t hblank_lo:8;
 uint32_t hblank_hi:4;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint32_t hsync_offset_lo:8;
 uint16_t hsync_offset_hi:2;
 uint16_t hsync_pulse_width_lo:8;
 uint16_t hsync_pulse_width_hi:2;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint16_t hsync_positive:1;
 uint16_t rsvd_1:3;
 uint8_t vactive_lo:8;
 uint16_t vactive_hi:4;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint16_t vblank_lo:8;
 uint16_t vblank_hi:4;
 uint16_t vsync_offset_lo:4;
 uint16_t vsync_offset_hi:2;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint16_t vsync_pulse_width_lo:4;
 uint16_t vsync_pulse_width_hi:2;
 uint16_t vsync_positive:1;
 uint16_t rsvd_2:3;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
} __attribute__((packed));
struct mrst_panel_descriptor_v1 {
 uint32_t Panel_Port_Control;
 uint32_t Panel_Power_On_Sequencing;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint32_t Panel_Power_Off_Sequencing;
 uint32_t Panel_Power_Cycle_Delay_and_Reference_Divisor;
 struct mrst_timing_info DTD;
 uint16_t Panel_Backlight_Inverter_Descriptor;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint16_t Panel_MIPI_Display_Descriptor;
} __attribute__((packed));
struct mrst_panel_descriptor_v2 {
 uint32_t Panel_Port_Control;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint32_t Panel_Power_On_Sequencing;
 uint32_t Panel_Power_Off_Sequencing;
 uint8_t Panel_Power_Cycle_Delay_and_Reference_Divisor;
 struct mrst_timing_info DTD;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint16_t Panel_Backlight_Inverter_Descriptor;
 uint8_t Panel_Initial_Brightness;
 uint16_t Panel_MIPI_Display_Descriptor;
} __attribute__((packed));
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
union mrst_panel_rx {
 struct {
 uint16_t NumberOfLanes:2;
 uint16_t MaxLaneFreq:3;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint16_t SupportedVideoTransferMode:2;
 uint16_t HSClkBehavior:1;
 uint16_t DuoDisplaySupport:1;
 uint16_t ECC_ChecksumCapabilities:1;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint16_t BidirectionalCommunication:1;
 uint16_t Rsvd:5;
 } panelrx;
 uint16_t panel_receiver;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
} __attribute__((packed));
struct gct_ioctl_arg {
 uint8_t bpi;
 uint8_t pt;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 struct mrst_timing_info DTD;
 uint32_t Panel_Port_Control;
 uint32_t PP_On_Sequencing;
 uint32_t PP_Off_Sequencing;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint32_t PP_Cycle_Delay;
 uint16_t Panel_Backlight_Inverter_Descriptor;
 uint16_t Panel_MIPI_Display_Descriptor;
} __attribute__((packed));
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
struct gct_timing_desc_block {
 uint16_t clock;
 uint16_t hactive:12;
 uint16_t hblank:12;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint16_t hsync_start:10;
 uint16_t hsync_end:10;
 uint16_t hsync_polarity:1;
 uint16_t h_reversed:3;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint16_t vactive:12;
 uint16_t vblank:12;
 uint16_t vsync_start:6;
 uint16_t vsync_end:6;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint16_t vsync_polarity:1;
 uint16_t v_reversed:3;
} __packed;
struct gct_display_desc_block {
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint8_t type:2;
 uint8_t pxiel_format:4;
 uint8_t mode:2;
 uint8_t frame_rate:6;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint8_t reserved:2;
} __attribute__((packed));
struct gct_dsi_desc_block {
 uint8_t lane_count:2;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint8_t lane_frequency:3;
 uint8_t transfer_mode:2;
 uint8_t hs_clock_behavior:1;
 uint8_t duo_display_support:1;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint8_t ecc_caps:1;
 uint8_t bdirect_support:1;
 uint8_t reversed:5;
} __packed;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
struct gct_bkl_desc_block {
 uint16_t frequency;
 uint8_t max_brightness:7;
 uint8_t polarity:1;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
} __packed;
struct gct_r20_clock_desc {
 uint8_t pre_divisor:2;
 uint16_t divisor:9;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint8_t post_divisor:4;
 uint8_t pll_bypass:1;
 uint8_t cck_clock_divisor:1;
 uint8_t reserved:7;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
} __packed;
struct gct_r20_panel_info {
 uint16_t width;
 uint16_t height;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
} __packed;
struct gct_r20_panel_mode {
 uint8_t mode:1;
 uint16_t reserved:15;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
} __packed;
struct gct_r20_dsi_desc {
 uint8_t num_dsi_lanes:2;
 uint16_t reserved:14;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
} __packed;
struct gct_r20_panel_desc {
 uint8_t panel_name[16];
 struct gct_timing_desc_block timing;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 struct gct_r20_clock_desc clock_desc;
 struct gct_r20_panel_info panel_info;
 struct gct_r20_panel_mode panel_mode;
 struct gct_r20_dsi_desc dsi_desc;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint32_t early_init_seq;
 uint32_t late_init_seq;
} __packed;
struct gct_r11_panel_desc {
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint8_t panel_name[16];
 struct gct_timing_desc_block timing;
 struct gct_display_desc_block display;
 struct gct_dsi_desc_block dsi;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 struct gct_bkl_desc_block bkl;
 uint32_t early_init_seq;
 uint32_t late_init_seq;
} __packed;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
struct gct_r10_panel_desc {
 struct gct_timing_desc_block timing;
 struct gct_display_desc_block display;
 struct gct_dsi_desc_block dsi;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 struct gct_bkl_desc_block bkl;
 uint32_t early_init_seq;
 uint32_t late_init_seq;
 uint8_t reversed[4];
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
} __packed;
struct intel_mid_vbt {
 char signature[4];
 uint8_t revision;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint8_t checksum;
 uint16_t size;
 uint8_t num_of_panel_desc;
 uint8_t primary_panel_idx;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint8_t secondary_panel_idx;
 uint8_t splash_flag;
 uint8_t reserved[4];
 void *panel_descs;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
} __packed;
struct mrst_vbt {
 char Signature[4];
 uint8_t Revision;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint8_t Size;
 uint8_t Checksum;
 void *mrst_gct;
} __attribute__ ((packed));
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
struct mrst_gct_v1 {
 union {
 struct {
 uint8_t PanelType:4;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint8_t BootPanelIndex:2;
 uint8_t BootMIPI_DSI_RxIndex:2;
 } PD;
 uint8_t PanelDescriptor;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 };
 struct mrst_panel_descriptor_v1 panel[4];
 union mrst_panel_rx panelrx[4];
} __attribute__((packed));
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
struct mrst_gct_v2 {
 union {
 struct {
 uint8_t PanelType:4;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint8_t BootPanelIndex:2;
 uint8_t BootMIPI_DSI_RxIndex:2;
 } PD;
 uint8_t PanelDescriptor;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 };
 struct mrst_panel_descriptor_v2 panel[4];
 union mrst_panel_rx panelrx[4];
} __attribute__((packed));
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define PSB_DC_CRTC_SAVE 0x01
#define PSB_DC_CRTC_RESTORE 0x02
#define PSB_DC_OUTPUT_SAVE 0x04
#define PSB_DC_OUTPUT_RESTORE 0x08
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define PSB_DC_CRTC_MASK 0x03
#define PSB_DC_OUTPUT_MASK 0x0C
struct drm_psb_dc_state_arg {
 uint32_t flags;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint32_t obj_id;
};
struct drm_psb_mode_operation_arg {
 uint32_t obj_id;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint16_t operation;
 struct drm_mode_modeinfo mode;
 void *data;
};
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
struct drm_psb_stolen_memory_arg {
 uint32_t base;
 uint32_t size;
};
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define REGRWBITS_PFIT_CONTROLS (1 << 0)
#define REGRWBITS_PFIT_AUTOSCALE_RATIOS (1 << 1)
#define REGRWBITS_PFIT_PROGRAMMED_SCALE_RATIOS (1 << 2)
#define REGRWBITS_PIPEASRC (1 << 3)
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define REGRWBITS_PIPEBSRC (1 << 4)
#define REGRWBITS_VTOTAL_A (1 << 5)
#define REGRWBITS_VTOTAL_B (1 << 6)
#define REGRWBITS_DSPACNTR (1 << 8)
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define REGRWBITS_DSPBCNTR (1 << 9)
#define REGRWBITS_DSPCCNTR (1 << 10)
#define REGRWBITS_SPRITE_UPDATE (1 << 11)
#define REGRWBITS_PIPEASTAT (1 << 12)
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define REGRWBITS_INT_MASK (1 << 13)
#define REGRWBITS_INT_ENABLE (1 << 14)
#define REGRWBITS_DISPLAY_ALL (1 << 15)
#define OV_REGRWBITS_OVADD (1 << 0)
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define OV_REGRWBITS_OGAM_ALL (1 << 1)
#define OVC_REGRWBITS_OVADD (1 << 2)
#define OVC_REGRWBITS_OGAM_ALL (1 << 3)
#define OV_REGRWBITS_WAIT_FLIP (1 << 4)
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define OVC_REGRWBITS_WAIT_FLIP (1 << 5)
#define OVSTATUS_REGRBIT_OVR_UPDT (1 << 6)
#define SPRITE_UPDATE_SURFACE (0x00000001UL)
#define SPRITE_UPDATE_CONTROL (0x00000002UL)
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define SPRITE_UPDATE_POSITION (0x00000004UL)
#define SPRITE_UPDATE_SIZE (0x00000008UL)
#define SPRITE_UPDATE_WAIT_VBLANK (0X00000010UL)
#define SPRITE_UPDATE_CONSTALPHA (0x00000020UL)
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define SPRITE_UPDATE_ALL (0x0000003fUL)
#define VSYNC_ENABLE (1 << 0)
#define VSYNC_DISABLE (1 << 1)
#define VSYNC_WAIT (1 << 2)
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define GET_VSYNC_COUNT (1 << 3)
#define N_HORIZ_Y_TAPS 5
#define N_VERT_Y_TAPS 3
#define N_HORIZ_UV_TAPS 3
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define N_VERT_UV_TAPS 3
#define N_PHASES 17
#define MAX_TAPS 5
struct overlay_ctrl_blk {
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint32_t OBUF_0Y;
 uint32_t OBUF_1Y;
 uint32_t OBUF_0U;
 uint32_t OBUF_0V;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint32_t OBUF_1U;
 uint32_t OBUF_1V;
 uint32_t OSTRIDE;
 uint32_t YRGB_VPH;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint32_t UV_VPH;
 uint32_t HORZ_PH;
 uint32_t INIT_PHS;
 uint32_t DWINPOS;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint32_t DWINSZ;
 uint32_t SWIDTH;
 uint32_t SWIDTHSW;
 uint32_t SHEIGHT;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint32_t YRGBSCALE;
 uint32_t UVSCALE;
 uint32_t OCLRC0;
 uint32_t OCLRC1;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint32_t DCLRKV;
 uint32_t DCLRKM;
 uint32_t SCHRKVH;
 uint32_t SCHRKVL;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint32_t SCHRKEN;
 uint32_t OCONFIG;
 uint32_t OCMD;
 uint32_t RESERVED1;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint32_t OSTART_0Y;
 uint32_t OSTART_1Y;
 uint32_t OSTART_0U;
 uint32_t OSTART_0V;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint32_t OSTART_1U;
 uint32_t OSTART_1V;
 uint32_t OTILEOFF_0Y;
 uint32_t OTILEOFF_1Y;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint32_t OTILEOFF_0U;
 uint32_t OTILEOFF_0V;
 uint32_t OTILEOFF_1U;
 uint32_t OTILEOFF_1V;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint32_t FASTHSCALE;
 uint32_t UVSCALEV;
 uint32_t RESERVEDC[(0x200 - 0xA8) / 4];
 uint16_t Y_VCOEFS[N_VERT_Y_TAPS * N_PHASES];
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint16_t RESERVEDD[0x100 / 2 - N_VERT_Y_TAPS * N_PHASES];
 uint16_t Y_HCOEFS[N_HORIZ_Y_TAPS * N_PHASES];
 uint16_t RESERVEDE[0x200 / 2 - N_HORIZ_Y_TAPS * N_PHASES];
 uint16_t UV_VCOEFS[N_VERT_UV_TAPS * N_PHASES];
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint16_t RESERVEDF[0x100 / 2 - N_VERT_UV_TAPS * N_PHASES];
 uint16_t UV_HCOEFS[N_HORIZ_UV_TAPS * N_PHASES];
 uint16_t RESERVEDG[0x100 / 2 - N_HORIZ_UV_TAPS * N_PHASES];
};
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
struct intel_overlay_context {
 uint32_t index;
 uint32_t pipe;
 uint32_t ovadd;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
};
struct intel_sprite_context {
 uint32_t update_mask;
 uint32_t index;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint32_t pipe;
 uint32_t cntr;
 uint32_t linoff;
 uint32_t stride;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint32_t pos;
 uint32_t size;
 uint32_t keyminval;
 uint32_t keymask;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint32_t surf;
 uint32_t keymaxval;
 uint32_t tileoff;
 uint32_t contalpa;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
};
#define INTEL_SPRITE_PLANE_NUM 3
#define INTEL_OVERLAY_PLANE_NUM 2
#define INTEL_DISPLAY_PLANE_NUM 5
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define INTEL_MDFLD_SPRITE_PLANE_NUM 3
#define INTEL_MDFLD_OVERLAY_PLANE_NUM 2
#define INTEL_MDFLD_CURSOR_PLANE_NUM 3
#define INTEL_MDFLD_DISPLAY_PLANE_NUM 8
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define INTEL_MDFLD_DISPLAY_PIPE_NUM 3
#define INTEL_CTP_SPRITE_PLANE_NUM 2
#define INTEL_CTP_OVERLAY_PLANE_NUM 1
#define INTEL_CTP_CURSOR_PLANE_NUM 2
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define INTEL_CTP_DISPLAY_PLANE_NUM 5
#define INTEL_CTP_DISPLAY_PIPE_NUM 2
#define INVALID_INDEX 0xffffffff
struct mdfld_plane_contexts {
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint32_t active_primaries;
 uint32_t active_sprites;
 uint32_t active_overlays;
 struct intel_sprite_context primary_contexts[INTEL_SPRITE_PLANE_NUM];
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 struct intel_sprite_context sprite_contexts[INTEL_SPRITE_PLANE_NUM];
 struct intel_overlay_context overlay_contexts[INTEL_OVERLAY_PLANE_NUM];
};
struct drm_psb_vsync_set_arg {
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint32_t vsync_operation_mask;
 struct {
 uint32_t pipe;
 int vsync_pipe;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 int vsync_count;
 uint64_t timestamp;
 } vsync;
};
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
struct drm_psb_dc_info {
 uint32_t pipe_count;
 uint32_t primary_plane_count;
 uint32_t sprite_plane_count;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint32_t overlay_plane_count;
 uint32_t cursor_plane_count;
};
struct drm_psb_register_rw_arg {
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint32_t b_force_hw_on;
 uint32_t display_read_mask;
 uint32_t display_write_mask;
 struct {
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint32_t pfit_controls;
 uint32_t pfit_autoscale_ratios;
 uint32_t pfit_programmed_scale_ratios;
 uint32_t pipeasrc;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint32_t pipebsrc;
 uint32_t vtotal_a;
 uint32_t vtotal_b;
 uint32_t dspcntr_a;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint32_t dspcntr_b;
 uint32_t pipestat_a;
 uint32_t int_mask;
 uint32_t int_enable;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 } display;
 uint32_t overlay_read_mask;
 uint32_t overlay_write_mask;
 struct {
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint32_t OVADD;
 uint32_t OGAMC0;
 uint32_t OGAMC1;
 uint32_t OGAMC2;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint32_t OGAMC3;
 uint32_t OGAMC4;
 uint32_t OGAMC5;
 uint32_t IEP_ENABLED;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint32_t IEP_BLE_MINMAX;
 uint32_t IEP_BSSCC_CONTROL;
 uint32_t index;
 uint32_t b_wait_vblank;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint32_t b_wms;
 uint32_t buffer_handle;
 uint32_t backbuf_index;
 unsigned long backbuf_addr;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 } overlay;
 uint32_t vsync_operation_mask;
 struct {
 uint32_t pipe;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 int vsync_pipe;
 int vsync_count;
 uint64_t timestamp;
 } vsync;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint32_t sprite_enable_mask;
 uint32_t sprite_disable_mask;
 struct {
 uint32_t dspa_control;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint32_t dspa_key_value;
 uint32_t dspa_key_mask;
 uint32_t dspc_control;
 uint32_t dspc_stride;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint32_t dspc_position;
 uint32_t dspc_linear_offset;
 uint32_t dspc_size;
 uint32_t dspc_surface;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 } sprite;
 uint32_t subpicture_enable_mask;
 uint32_t subpicture_disable_mask;
 struct {
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint32_t CursorADDR;
 uint32_t xPos;
 uint32_t yPos;
 uint32_t CursorSize;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 } cursor;
 uint32_t cursor_enable_mask;
 uint32_t cursor_disable_mask;
 uint32_t plane_enable_mask;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint32_t plane_disable_mask;
 struct {
 uint32_t type;
 uint32_t index;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint32_t ctx;
 } plane;
};
enum {
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 PSB_DC_PLANE_ENABLED,
 PSB_DC_PLANE_DISABLED,
};
enum {
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 PSB_GTT_MAP_TYPE_MEMINFO = 0,
 PSB_GTT_MAP_TYPE_BCD,
 PSB_GTT_MAP_TYPE_BCD_INFO,
 PSB_GTT_MAP_TYPE_VIRTUAL,
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
};
struct psb_gtt_mapping_arg {
 uint32_t type;
 void *hKernelMemInfo;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint32_t offset_pages;
 uint32_t page_align;
 uint32_t bcd_device_id;
 uint32_t bcd_buffer_id;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint32_t bcd_buffer_count;
 uint32_t bcd_buffer_stride;
 uint32_t vaddr;
 uint32_t size;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
};
struct drm_psb_getpageaddrs_arg {
 uint32_t handle;
 unsigned long *page_addrs;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 unsigned long gtt_offset;
};
#define MAX_SLICES_PER_PICTURE 72
struct psb_msvdx_mb_region {
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint32_t start;
 uint32_t end;
};
typedef struct drm_psb_msvdx_decode_status {
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint32_t num_region;
 struct psb_msvdx_mb_region mb_regions[MAX_SLICES_PER_PICTURE];
} drm_psb_msvdx_decode_status_t;
#define DRM_PSB_KMS_OFF 0x00
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define DRM_PSB_KMS_ON 0x01
#define DRM_PSB_VT_LEAVE 0x02
#define DRM_PSB_VT_ENTER 0x03
#define DRM_PSB_EXTENSION 0x06
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define DRM_PSB_SIZES 0x07
#define DRM_PSB_FUSE_REG 0x08
#define DRM_PSB_VBT 0x09
#define DRM_PSB_DC_STATE 0x0A
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define DRM_PSB_ADB 0x0B
#define DRM_PSB_MODE_OPERATION 0x0C
#define DRM_PSB_STOLEN_MEMORY 0x0D
#define DRM_PSB_REGISTER_RW 0x0E
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define DRM_PSB_GTT_MAP 0x0F
#define DRM_PSB_GTT_UNMAP 0x10
#define DRM_PSB_GETPAGEADDRS 0x11
#define DRM_PVR_RESERVED1 0x12
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define DRM_PVR_RESERVED2 0x13
#define DRM_PVR_RESERVED3 0x14
#define DRM_PVR_RESERVED4 0x15
#define DRM_PVR_RESERVED5 0x16
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define DRM_PSB_HIST_ENABLE 0x17
#define DRM_PSB_HIST_STATUS 0x18
#define DRM_PSB_UPDATE_GUARD 0x19
#define DRM_PSB_INIT_COMM 0x1A
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define DRM_PSB_DPST 0x1B
#define DRM_PSB_GAMMA 0x1C
#define DRM_PSB_DPST_BL 0x1D
#define DRM_PVR_RESERVED6 0x1E
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define DRM_PSB_GET_PIPE_FROM_CRTC_ID 0x1F
#define DRM_PSB_DPU_QUERY 0x20
#define DRM_PSB_DPU_DSR_ON 0x21
#define DRM_PSB_DPU_DSR_OFF 0x22
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define DRM_PSB_HDMI_FB_CMD 0x23
#define DRM_PSB_QUERY_HDCP 0x24
#define DRM_PSB_VALIDATE_HDCP_KSV 0x25
#define DRM_PSB_GET_HDCP_STATUS 0x26
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define DRM_PSB_ENABLE_HDCP 0x27
#define DRM_PSB_DISABLE_HDCP 0x28
#define DRM_PSB_GET_HDCP_LINK_STATUS 0x2b
#define DRM_PSB_ENABLE_HDCP_REPEATER 0x2c
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define DRM_PSB_DISABLE_HDCP_REPEATER 0x2d
#define DRM_PSB_HDCP_REPEATER_PRESENT 0x2e
#define DRM_PSB_CSC_GAMMA_SETTING 0x29
#define DRM_PSB_SET_CSC 0x2a
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define DRM_PSB_ENABLE_IED_SESSION 0x30
#define DRM_PSB_DISABLE_IED_SESSION 0x31
#define DRM_PSB_VSYNC_SET 0x32
#define DRM_PSB_HDCP_DISPLAY_IED_OFF 0x33
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define DRM_PSB_HDCP_DISPLAY_IED_ON 0x34
#define DRM_PSB_QUERY_HDCP_DISPLAY_IED_CAPS 0x35
#define DRM_PSB_DPST_LEVEL 0x36
#define DRM_PSB_GET_DC_INFO 0x37
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define DRM_PSB_HDMITEST 0x38
#define HT_READ 1
#define HT_WRITE 2
#define HT_FORCEON 4
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
typedef struct tagHDMITESTREGREADWRITE {
 unsigned int reg;
 unsigned int data;
 int mode;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
} drm_psb_hdmireg_t, *drm_psb_hdmireg_p;
#define DRM_OEM_RESERVED_START 0x40
#define DRM_OEM_RESERVED_END 0x4F
#define DRM_PSB_TTM_START 0x50
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define DRM_PSB_TTM_END 0x5F
#ifdef PDUMP
#define DRM_PSB_CMDBUF (PVR_DRM_DBGDRV_CMD + 1)
#else
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define DRM_PSB_CMDBUF (DRM_PSB_TTM_START)
#endif
#define DRM_PSB_SCENE_UNREF (DRM_PSB_CMDBUF + 1)
#define DRM_PSB_PLACEMENT_OFFSET (DRM_PSB_SCENE_UNREF + 1)
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define DRM_PSB_DSR_ENABLE 0xfffffffe
#define DRM_PSB_DSR_DISABLE 0xffffffff
struct drm_psb_csc_matrix {
 int pipe;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 int64_t matrix[9];
};
struct psb_drm_dpu_rect {
 int x, y;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 int width, height;
};
struct drm_psb_drv_dsr_off_arg {
 int screen;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 struct psb_drm_dpu_rect damage_rect;
};
struct drm_psb_dev_info_arg {
 uint32_t num_use_attribute_registers;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
};
#define DRM_PSB_DEVINFO 0x01
#define PSB_MODE_OPERATION_MODE_VALID 0x01
#define PSB_MODE_OPERATION_SET_DC_BASE 0x02
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
struct drm_psb_get_pipe_from_crtc_id_arg {
 uint32_t crtc_id;
 uint32_t pipe;
};
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define DRM_PSB_DISP_SAVE_HDMI_FB_HANDLE 1
#define DRM_PSB_DISP_GET_HDMI_FB_HANDLE 2
#define DRM_PSB_DISP_INIT_HDMI_FLIP_CHAIN 1
#define DRM_PSB_DISP_QUEUE_BUFFER 2
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define DRM_PSB_DISP_DEQUEUE_BUFFER 3
#define DRM_PSB_DISP_PLANEB_DISABLE 4
#define DRM_PSB_DISP_PLANEB_ENABLE 5
#define DRM_PSB_HDMI_OSPM_ISLAND_DOWN 6
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define DRM_PSB_HDMI_NOTIFY_HOTPLUG_TO_AUDIO 7
typedef enum {
 GAMMA,
 CSC,
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 GAMMA_INITIA,
 GAMMA_SETTING,
 GAMMA_REG_SETTING,
 CSC_INITIA,
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 CSC_CHROME_SETTING,
 CSC_SETTING,
 CSC_REG_SETTING
} setting_type;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
typedef enum {
 GAMMA_05 = 1,
 GAMMA_20,
 GAMMA_05_20,
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 GAMMA_20_05,
 GAMMA_10
} gamma_mode;
#define CSC_REG_COUNT 6
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define CHROME_COUNT 16
#define CSC_COUNT 9
struct csc_setting {
 uint32_t pipe;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 setting_type type;
 bool enable_state;
 uint32_t data_len;
 union {
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 int csc_reg_data[CSC_REG_COUNT];
 int chrome_data[CHROME_COUNT];
 int64_t csc_data[CSC_COUNT];
 } data;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
};
#define GAMMA_10_BIT_TABLE_COUNT 129
struct gamma_setting {
 uint32_t pipe;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 setting_type type;
 bool enable_state;
 gamma_mode initia_mode;
 uint32_t data_len;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint32_t gamma_tableX100[GAMMA_10_BIT_TABLE_COUNT];
};
struct drm_psb_csc_gamma_setting {
 setting_type type;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 union {
 struct csc_setting csc_data;
 struct gamma_setting gamma_data;
 } data;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
};
struct drm_psb_buffer_data {
 void *h_buffer;
};
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
struct drm_psb_flip_chain_data {
 void **h_buffer_array;
 unsigned int size;
};
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
struct drm_psb_disp_ctrl {
 uint32_t cmd;
 union {
 uint32_t data;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 struct drm_psb_buffer_data buf_data;
 struct drm_psb_flip_chain_data flip_chain_data;
 } u;
};
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define S3D_MIPIA_DISPLAY 0
#define S3D_HDMI_DISPLAY 1
#define S3D_MIPIC_DISPLAY 2
#define S3D_WIDI_DISPLAY 0xFF
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
struct drm_psb_s3d_query {
 uint32_t s3d_display_type;
 uint32_t is_s3d_supported;
 uint32_t s3d_format;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint32_t mode_resolution_x;
 uint32_t mode_resolution_y;
 uint32_t mode_refresh_rate;
 uint32_t is_interleaving;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
};
struct drm_psb_s3d_premodeset {
 uint32_t s3d_buffer_format;
};
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
typedef enum intel_dc_plane_types {
 DC_UNKNOWN_PLANE = 0,
 DC_SPRITE_PLANE = 1,
 DC_OVERLAY_PLANE,
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 DC_PRIMARY_PLANE,
 DC_PLANE_MAX,
} DC_MRFLD_PLANE_TYPE;
#define SPRITE_UPDATE_SURFACE (0x00000001UL)
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define SPRITE_UPDATE_CONTROL (0x00000002UL)
#define SPRITE_UPDATE_POSITION (0x00000004UL)
#define SPRITE_UPDATE_SIZE (0x00000008UL)
#define SPRITE_UPDATE_WAIT_VBLANK (0X00000010UL)
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define SPRITE_UPDATE_CONSTALPHA (0x00000020UL)
#define SPRITE_UPDATE_ALL (0x0000003fUL)
#define MRFLD_PRIMARY_COUNT 3
typedef struct intel_dc_overlay_ctx {
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint32_t index;
 uint32_t pipe;
 uint32_t ovadd;
} DC_MRFLD_OVERLAY_CONTEXT;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
typedef struct intel_dc_sprite_ctx {
 uint32_t update_mask;
 uint32_t index;
 uint32_t pipe;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint32_t cntr;
 uint32_t linoff;
 uint32_t stride;
 uint32_t pos;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint32_t size;
 uint32_t keyminval;
 uint32_t keymask;
 uint32_t surf;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint32_t keymaxval;
 uint32_t tileoff;
 uint32_t contalpa;
} DC_MRFLD_SPRITE_CONTEXT;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
typedef struct intel_dc_primary_ctx {
 uint32_t update_mask;
 uint32_t index;
 uint32_t pipe;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint32_t cntr;
 uint32_t linoff;
 uint32_t stride;
 uint32_t pos;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint32_t size;
 uint32_t keyminval;
 uint32_t keymask;
 uint32_t surf;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 uint32_t keymaxval;
 uint32_t tileoff;
 uint32_t contalpa;
} DC_MRFLD_PRIMARY_CONTEXT;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
typedef struct intel_dc_plane_zorder {
 uint32_t forceBottom[3];
 uint32_t abovePrimary;
} DC_MRFLD_DC_PLANE_ZORDER;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
typedef struct intel_dc_plane_ctx {
 enum intel_dc_plane_types type;
 struct intel_dc_plane_zorder zorder;
 union {
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 struct intel_dc_overlay_ctx ov_ctx;
 struct intel_dc_sprite_ctx sp_ctx;
 struct intel_dc_primary_ctx prim_ctx;
 } ctx;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
} DC_MRFLD_SURF_CUSTOM;
#endif
