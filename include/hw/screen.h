#pragma once

#include "common.h"
#include "hw/memory_map.h"

/*
 * Omni10 screen / framebuffer API (own implementation)
 */

typedef enum {
    OMNI_SCREEN_TOP = 0,
    OMNI_SCREEN_BOTTOM = 1
} omni_screen_t;

omni_result_t screen_init(void);
void screen_deinit(void);

void screen_clear(omni_screen_t which, uint16_t rgb565);
void screen_swap(void);

uint16_t* screen_get_fb(omni_screen_t which);
int screen_width(omni_screen_t which);
int screen_height(omni_screen_t which);

/* Simple pixel + text helpers (bitmap font later) */
void screen_put_pixel(omni_screen_t which, int x, int y, uint16_t rgb565);
void screen_fill_rect(omni_screen_t which, int x, int y, int w, int h, uint16_t rgb565);
