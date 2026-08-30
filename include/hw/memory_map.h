#pragma once

/*
 * Omni10-3DS – Physical memory map
 * Public 3DS layout for our own FIRM (not from GM9 source)
 */

#include <stdint.h>

/* ---- IO ---- */
#define OMNI_IO_BASE           0x10000000u

/* ---- VRAM ---- */
#define OMNI_VRAM_BASE         0x18000000u
#define OMNI_VRAM_SIZE         0x00600000u  /* 6 MB */

/* ---- DSP / AXI ---- */
#define OMNI_DSP_BASE          0x1FF00000u
#define OMNI_AXI_WRAM_BASE     0x1FF80000u
#define OMNI_AXI_WRAM_SIZE     0x00080000u  /* 512 KB */

/* ---- FCRAM ---- */
#define OMNI_FCRAM_BASE        0x20000000u
#define OMNI_FCRAM_SIZE_O3DS   0x08000000u  /* 128 MB */
#define OMNI_FCRAM_EXT_N3DS    0x28000000u  /* +128 MB N3DS */

/* ---- ARM9 RAM ---- */
#define OMNI_ARM9_RAM_BASE     0x08000000u
#define OMNI_ARM9_RAM_SIZE     0x00100000u  /* 1 MB */
#define OMNI_ARM9_RAM_EXT_N3DS 0x08100000u  /* 512 KB */

/* ---- ARM11 MPCore ---- */
#define OMNI_MPCORE_BASE       0x17E00000u

/* ---- Screens (logical) ---- */
#define OMNI_SCREEN_TOP_W      400
#define OMNI_SCREEN_TOP_H      240
#define OMNI_SCREEN_BOT_W      320
#define OMNI_SCREEN_BOT_H      240

/* Bytes per pixel when using RGB565 */
#define OMNI_BPP_RGB565        2

#define OMNI_TOP_FB_SIZE       (OMNI_SCREEN_TOP_W * OMNI_SCREEN_TOP_H * OMNI_BPP_RGB565)
#define OMNI_BOT_FB_SIZE       (OMNI_SCREEN_BOT_W * OMNI_SCREEN_BOT_H * OMNI_BPP_RGB565)

/*
 * Framebuffer physical bases are set at runtime after GPU/LCD init.
 * Placeholders until screen driver exists:
 */
#define OMNI_FB_TOP_PLACEHOLDER  0x18300000u
#define OMNI_FB_BOT_PLACEHOLDER  0x18346500u
