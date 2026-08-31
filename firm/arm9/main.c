/*
 * Omni10 – diagnostic draw (fix FB orientation)
 * Top physical: 400x240
 * Try mapping: fb[x * 240 + y]
 */

#include <stdint.h>
#include <stddef.h>

#define REG_PAD_HID (*(volatile uint32_t *)0x10146000)

#define SW 400
#define SH 240
#define ST 240

#define FB_TOP ((volatile uint16_t *)0x18300000)
#define FB_BOT ((volatile uint16_t *)0x18346500)

#define BTN_LEFT  (1u << 5)
#define BTN_RIGHT (1u << 4)
#define BTN_UP    (1u << 6)
#define BTN_DOWN  (1u << 7)
#define BTN_A     (1u << 0)

#define RGB565(r, g, b) ((uint16_t)(((r) & 0x1F) << 11 | ((g) & 0x3F) << 5 | ((b) & 0x1F)))

struct fb {
    uint8_t *top_left;
    uint8_t *top_right;
    uint8_t *bottom;
};

static volatile uint16_t *g_top;
static volatile uint16_t *g_bot;

/* 0 = x*240+y, 1 = x*240+(239-y), 2 = y*400+x (unlikely) */
static int g_map_mode;

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
    if (!g_top || x < 0 || y < 0 || x >= SW || y >= SH)
        return;
    int idx;
    if (g_map_mode == 1)
        idx = x * ST + (SH - 1 - y);
    else if (g_map_mode == 2)
        idx = y * SW + x;
    else
        idx = x * ST + y;
    if (idx >= 0 && idx < SW * ST)
        g_top[idx] = c;
}

static void fill(uint16_t c)
{
    if (!g_top)
        return;
    for (int i = 0; i < SW * ST; i++)
        g_top[i] = c;
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
    g_map_mode = 0;

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
    const uint16_t CYN = RGB565(0, 63, 31);

    int bar_x = 150;

    while (1) {
        fill(BLK);

        /* 4 quadrants – must look like solid color blocks if mapping is right */
        rect(0, 0, 200, 120, RED);
        rect(200, 0, 200, 120, GRN);
        rect(0, 120, 200, 120, BLU);
        rect(200, 120, 200, 120, WHT);

        /* movable cyan bar on top of pattern */
        rect(bar_x, 100, 80, 30, CYN);

        if (g_bot) {
            for (int i = 0; i < 240 * 320; i++)
                g_bot[i] = BLK;
        }

        drain();

        uint32_t k = ~REG_PAD_HID;
        if (k & BTN_LEFT) {
            bar_x -= 5;
            if (bar_x < 0)
                bar_x = 0;
        }
        if (k & BTN_RIGHT) {
            bar_x += 5;
            if (bar_x > SW - 80)
                bar_x = SW - 80;
        }
        /* A cycles mapping mode 0/1/2 for live debug */
        if (k & BTN_A) {
            g_map_mode = (g_map_mode + 1) % 3;
            delay(300000);
        }

        delay(10000);
    }

    return 0;
}
