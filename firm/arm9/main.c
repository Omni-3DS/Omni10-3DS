/*
 * Omni10 draw – standard 3DS top FB layout:
 *   PIXEL_OFFSET(x,y) = x * 240 + (239 - y)
 *   x: 0..399  y: 0..239
 */

#include <stdint.h>
#include <stddef.h>

#define REG_PAD_HID (*(volatile uint32_t *)0x10146000)

#define SCREEN_W 400
#define SCREEN_H 240

#define FB_TOP ((volatile uint16_t *)0x18300000)
#define FB_BOT ((volatile uint16_t *)0x18346500)

#define BTN_LEFT  (1u << 5)
#define BTN_RIGHT (1u << 4)

#define RGB565(r, g, b) ((uint16_t)(((r) & 0x1F) << 11 | ((g) & 0x3F) << 5 | ((b) & 0x1F)))

#define PIXEL_OFFSET(x, y) (((x) * SCREEN_H) + (SCREEN_H - (y) - 1))

struct fb {
    uint8_t *top_left;
    uint8_t *top_right;
    uint8_t *bottom;
};

static volatile uint16_t *g_top;
static volatile uint16_t *g_bot;

void *memset(void *dest, int val, size_t count)
{
    uint8_t *p = (uint8_t *)dest;
    while (count--)
        *p++ = (uint8_t)val;
    return dest;
}

static void drain(void)
{
    uint32_t z = 0;
    __asm__ volatile("mcr p15, 0, %0, c7, c10, 4" ::"r"(z) : "memory");
}

static void delay(int n)
{
    for (volatile int i = 0; i < n; i++)
        __asm__ volatile("nop");
}

static void put(int x, int y, uint16_t c)
{
    if (!g_top || x < 0 || y < 0 || x >= SCREEN_W || y >= SCREEN_H)
        return;
    g_top[PIXEL_OFFSET(x, y)] = c;
}

/* Fast clear via 32-bit stores */
static void clear_top(uint16_t color)
{
    if (!g_top)
        return;
    uint32_t c32 = ((uint32_t)color << 16) | color;
    volatile uint32_t *p = (volatile uint32_t *)g_top;
    int n = (SCREEN_W * SCREEN_H) / 2;
    for (int i = 0; i < n; i++)
        p[i] = c32;
}

static void rect(int x, int y, int w, int h, uint16_t c)
{
    for (int j = 0; j < h; j++)
        for (int i = 0; i < w; i++)
            put(x + i, y + j, c);
}

int main(int argc, char **argv)
{
    g_top = FB_TOP;
    g_bot = FB_BOT;

    if (argc >= 2 && argv && argv[1]) {
        struct fb *fbs = (struct fb *)argv[1];
        if (fbs[0].top_left)
            g_top = (volatile uint16_t *)fbs[0].top_left;
        if (fbs[0].bottom)
            g_bot = (volatile uint16_t *)fbs[0].bottom;
    }

    const uint16_t BLK = RGB565(0, 0, 0);
    const uint16_t RED = RGB565(31, 0, 0);
    const uint16_t GRN = RGB565(0, 63, 0);
    const uint16_t BLU = RGB565(0, 0, 31);
    const uint16_t WHT = RGB565(31, 63, 31);
    const uint16_t CYN = RGB565(0, 50, 31);

    /* Static background once – no full-screen flash every frame */
    clear_top(BLK);
    rect(0, 0, 200, 120, RED);
    rect(200, 0, 200, 120, GRN);
    rect(0, 120, 200, 120, BLU);
    rect(200, 120, 200, 120, WHT);
    if (g_bot) {
        uint32_t c32 = 0;
        volatile uint32_t *p = (volatile uint32_t *)g_bot;
        for (int i = 0; i < (240 * 320) / 2; i++)
            p[i] = c32;
    }
    drain();

    int bar_x = 160;
    int prev_x = -1;
    const int bar_y = 105;
    const int bar_w = 80;
    const int bar_h = 28;

    while (1) {
        /* erase old bar by redrawing underlay colors */
        if (prev_x >= 0 && prev_x != bar_x) {
            for (int j = 0; j < bar_h; j++) {
                for (int i = 0; i < bar_w; i++) {
                    int px = prev_x + i;
                    int py = bar_y + j;
                    uint16_t under;
                    if (px < 200 && py < 120)
                        under = RED;
                    else if (px >= 200 && py < 120)
                        under = GRN;
                    else if (px < 200)
                        under = BLU;
                    else
                        under = WHT;
                    put(px, py, under);
                }
            }
        }

        rect(bar_x, bar_y, bar_w, bar_h, CYN);
        prev_x = bar_x;
        drain();

        uint32_t k = ~REG_PAD_HID;
        if (k & BTN_LEFT) {
            bar_x -= 4;
            if (bar_x < 0)
                bar_x = 0;
        }
        if (k & BTN_RIGHT) {
            bar_x += 4;
            if (bar_x > SCREEN_W - bar_w)
                bar_x = SCREEN_W - bar_w;
        }

        delay(8000);
    }

    return 0;
}
