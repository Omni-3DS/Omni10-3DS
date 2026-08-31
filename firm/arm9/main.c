/*
 * Omni10-3DS ARM9 – drawing in physical screen space
 * Top LCD: 400x240, FB stored rotated (stride 240)
 * pixel (sx,sy) -> fb[sx * 240 + (239 - sy)]
 */

#include <stdint.h>
#include <stddef.h>

#define REG_PAD_HID (*(volatile uint32_t *)0x10146000)

#define SCREEN_W 400
#define SCREEN_H 240
#define FB_STRIDE 240

#define FB_TOP_DEFAULT ((volatile uint16_t *)0x18300000)
#define FB_BOT_DEFAULT ((volatile uint16_t *)0x18346500)

#define BUTTON_LEFT  (1u << 5)
#define BUTTON_RIGHT (1u << 4)

#define RGB565(r, g, b) ((uint16_t)(((r) & 0x1F) << 11 | ((g) & 0x3F) << 5 | ((b) & 0x1F)))

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

static void drain_write_buffer(void)
{
    uint32_t zero = 0;
    __asm__ volatile("mcr p15, 0, %0, c7, c10, 4" ::"r"(zero) : "memory");
}

static void delay(int n)
{
    for (volatile int i = 0; i < n; i++)
        __asm__ volatile("nop");
}

/* Physical top-screen pixel */
static void put_top(int sx, int sy, uint16_t color)
{
    if (!g_top)
        return;
    if (sx < 0 || sx >= SCREEN_W || sy < 0 || sy >= SCREEN_H)
        return;
    g_top[sx * FB_STRIDE + (SCREEN_H - 1 - sy)] = color;
}

static void fill_top(uint16_t color)
{
    if (!g_top)
        return;
    for (int i = 0; i < SCREEN_W * FB_STRIDE; i++)
        g_top[i] = color;
}

static void rect_top(int x, int y, int w, int h, uint16_t color)
{
    for (int j = 0; j < h; j++)
        for (int i = 0; i < w; i++)
            put_top(x + i, y + j, color);
}

static void fill_bot(uint16_t color)
{
    if (!g_bot)
        return;
    /* Bottom: 320x240 physical, FB often 240 stride x 320 */
    for (int i = 0; i < 240 * 320; i++)
        g_bot[i] = color;
}

int main(int argc, char **argv)
{
    g_top = FB_TOP_DEFAULT;
    g_bot = FB_BOT_DEFAULT;

    if (argc >= 2 && argv && argv[1]) {
        struct fb *fbs = (struct fb *)argv[1];
        if (fbs[0].top_left)
            g_top = (volatile uint16_t *)fbs[0].top_left;
        if (fbs[0].bottom)
            g_bot = (volatile uint16_t *)fbs[0].bottom;
    }

    int bar_x = 170;
    const int bar_y = 100;
    const int bar_w = 60;
    const int bar_h = 24;

    const uint16_t COL_BG  = RGB565(0, 0, 0);
    const uint16_t COL_BAR = RGB565(0, 50, 31);
    const uint16_t COL_RED = RGB565(31, 0, 0);
    const uint16_t COL_GRN = RGB565(0, 63, 0);
    const uint16_t COL_WHT = RGB565(31, 63, 31);

    while (1) {
        fill_top(COL_BG);

        /* Corners of physical 400x240 screen */
        rect_top(0, 0, 20, 20, COL_RED);
        rect_top(SCREEN_W - 20, 0, 20, 20, COL_GRN);
        rect_top(0, SCREEN_H - 20, 20, 20, COL_WHT);
        rect_top(SCREEN_W - 20, SCREEN_H - 20, 20, 20, COL_WHT);

        rect_top(bar_x, bar_y, bar_w, bar_h, COL_BAR);

        fill_bot(COL_BG);

        drain_write_buffer();

        uint32_t k = ~REG_PAD_HID;
        if (k & BUTTON_LEFT) {
            bar_x -= 4;
            if (bar_x < 0)
                bar_x = 0;
        }
        if (k & BUTTON_RIGHT) {
            bar_x += 4;
            if (bar_x > SCREEN_W - bar_w)
                bar_x = SCREEN_W - bar_w;
        }

        delay(12000);
    }

    return 0;
}
