/*
 * Omni10-3DS ARM9 payload
 *
 * Luma3DS chainload passes:
 *   argv[0] = path
 *   argv[1] = pointer to framebuffers { top_left, top_right, bottom }
 * Writing to fixed 0x18000000 often freezes under Luma – use argv FBs.
 */

#include <stdint.h>
#include <stddef.h>

#define REG_PAD_HID (*(volatile uint32_t *)0x10146000)

#define TOP_W 240
#define TOP_H 400

#define BUTTON_LEFT  (1u << 5)
#define BUTTON_RIGHT (1u << 4)

#define RGB565(r, g, b) ((uint16_t)(((r) & 0x1F) << 11 | ((g) & 0x3F) << 5 | ((b) & 0x1F)))

static uint16_t *g_top;
static uint16_t *g_bot;

/* Required: gcc -O2 + -nostdlib may emit calls to memset */
void *memset(void *dest, int val, size_t count)
{
    uint8_t *ptr = (uint8_t *)dest;
    while (count--)
        *ptr++ = (uint8_t)val;
    return dest;
}

static void delay(int n)
{
    for (volatile int i = 0; i < n; i++)
        __asm__ volatile("nop");
}

static void fb_clear(uint16_t *fb, int w, int h, uint16_t color)
{
    if (!fb)
        return;
    int n = w * h;
    for (int i = 0; i < n; i++)
        fb[i] = color;
}

static void fb_rect(uint16_t *fb, int w, int h, int x, int y, int rw, int rh, uint16_t color)
{
    if (!fb)
        return;
    for (int j = 0; j < rh; j++) {
        for (int i = 0; i < rw; i++) {
            int px = x + i;
            int py = y + j;
            if (px >= 0 && py >= 0 && px < w && py < h)
                fb[py * w + px] = color;
        }
    }
}

int main(int argc, char **argv)
{
    g_top = (uint16_t *)0x18300000;
    g_bot = (uint16_t *)0x18346500;

    if (argc >= 2 && argv[1]) {
        uint32_t *fbs = (uint32_t *)argv[1];
        if (fbs[0])
            g_top = (uint16_t *)fbs[0];
        if (fbs[2])
            g_bot = (uint16_t *)fbs[2];
    }

    int bar_x = 100;
    const int bar_y = 200;
    const int bar_w = 40;
    const int bar_h = 12;

    const uint16_t COL_BG  = RGB565(0, 0, 0);
    const uint16_t COL_BAR = RGB565(0, 20, 31);
    const uint16_t COL_RED = RGB565(31, 0, 0);
    const uint16_t COL_GRN = RGB565(0, 63, 0);

    while (1) {
        fb_clear(g_top, TOP_W, TOP_H, COL_BG);
        fb_rect(g_top, TOP_W, TOP_H, bar_x, bar_y, bar_w, bar_h, COL_BAR);
        fb_rect(g_top, TOP_W, TOP_H, 0, 0, 10, 10, COL_RED);
        fb_rect(g_top, TOP_W, TOP_H, TOP_W - 10, 0, 10, 10, COL_GRN);

        if (g_bot)
            fb_clear(g_bot, 240, 320, COL_BG);

        uint32_t k = ~REG_PAD_HID;
        if (k & BUTTON_LEFT) {
            bar_x -= 4;
            if (bar_x < 0)
                bar_x = 0;
        }
        if (k & BUTTON_RIGHT) {
            bar_x += 4;
            if (bar_x > TOP_W - bar_w)
                bar_x = TOP_W - bar_w;
        }

        delay(25000);
    }

    return 0;
}
