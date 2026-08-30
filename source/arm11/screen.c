/*
 * Omni10-3DS – Screen / framebuffer (stub)
 * Own code – will drive real LCD/GPU later
 */

#include "hw/screen.h"
#include <string.h>

static uint16_t* fb_top = (uint16_t*)(uintptr_t)OMNI_FB_TOP_PLACEHOLDER;
static uint16_t* fb_bot = (uint16_t*)(uintptr_t)OMNI_FB_BOT_PLACEHOLDER;

omni_result_t screen_init(void)
{
    /* TODO: real GPU/LCD init, set fb pointers from hardware */
    return OMNI_OK;
}

void screen_deinit(void)
{
}

void screen_clear(omni_screen_t which, uint16_t rgb565)
{
    uint16_t* fb = screen_get_fb(which);
    int n = screen_width(which) * screen_height(which);
    if (!fb) return;
    for (int i = 0; i < n; i++)
        fb[i] = rgb565;
}

void screen_swap(void)
{
    /* TODO: flush + swap buffers */
}

uint16_t* screen_get_fb(omni_screen_t which)
{
    return (which == OMNI_SCREEN_TOP) ? fb_top : fb_bot;
}

int screen_width(omni_screen_t which)
{
    return (which == OMNI_SCREEN_TOP) ? OMNI_SCREEN_TOP_W : OMNI_SCREEN_BOT_W;
}

int screen_height(omni_screen_t which)
{
    return (which == OMNI_SCREEN_TOP) ? OMNI_SCREEN_TOP_H : OMNI_SCREEN_BOT_H;
}

void screen_put_pixel(omni_screen_t which, int x, int y, uint16_t rgb565)
{
    int w = screen_width(which);
    int h = screen_height(which);
    if (x < 0 || y < 0 || x >= w || y >= h) return;
    uint16_t* fb = screen_get_fb(which);
    if (!fb) return;
    fb[y * w + x] = rgb565;
}

void screen_fill_rect(omni_screen_t which, int x, int y, int w, int h, uint16_t rgb565)
{
    for (int j = 0; j < h; j++)
        for (int i = 0; i < w; i++)
            screen_put_pixel(which, x + i, y + j, rgb565);
}
