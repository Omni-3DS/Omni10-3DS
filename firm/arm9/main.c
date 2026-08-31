#include <stdint.h>
#include <stddef.h>

#define REG_PAD_HID (*(volatile uint32_t *)0x10146000)

#define TOP_W 240
#define TOP_H 400
#define BOT_W 240
#define BOT_H 320

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

static void fill16(volatile uint16_t *fb, int pixels, uint16_t color)
{
    if (!fb)
        return;
    for (int i = 0; i < pixels; i++)
        fb[i] = color;
}

static void rect16(volatile uint16_t *fb, int w, int h, int x, int y, int rw, int rh, uint16_t color)
{
    if (!fb)
        return;
    for (int j = 0; j < rh; j++) {
        int py = y + j;
        if (py < 0 || py >= h)
            continue;
        for (int i = 0; i < rw; i++) {
            int px = x + i;
            if (px >= 0 && px < w)
                fb[py * w + px] = color;
        }
    }
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

    int bar_x = 80;
    const int bar_y = 180;
    const int bar_w = 60;
    const int bar_h = 20;

    const uint16_t COL_BG  = RGB565(0, 0, 0);
    const uint16_t COL_BAR = RGB565(0, 50, 31);
    const uint16_t COL_RED = RGB565(31, 0, 0);
    const uint16_t COL_GRN = RGB565(0, 63, 0);
    const uint16_t COL_WHT = RGB565(31, 63, 31);

    while (1) {
        fill16(g_top, TOP_W * TOP_H, COL_BG);
        rect16(g_top, TOP_W, TOP_H, 0, 0, 16, 16, COL_RED);
        rect16(g_top, TOP_W, TOP_H, TOP_W - 16, 0, 16, 16, COL_GRN);
        rect16(g_top, TOP_W, TOP_H, 0, TOP_H - 16, 16, 16, COL_WHT);
        rect16(g_top, TOP_W, TOP_H, bar_x, bar_y, bar_w, bar_h, COL_BAR);

        if (g_bot)
            fill16(g_bot, BOT_W * BOT_H, COL_BG);

        drain_write_buffer();

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

        delay(15000);
    }

    return 0;
}
