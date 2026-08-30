/*
 * Omni10-3DS ARM9 payload
 * Same bare-metal pattern as SlabyLol/3DSFirm (proven loadable firm)
 */

#include <stdint.h>
#include <stddef.h>

#define REG_PDN_CLKEN0    (*(volatile uint32_t *)0x10141000)
#define REG_PAD_HID       (*(volatile uint32_t *)0x10146000)
#define TOP_SCREEN_PADDR  0x18000000

#define SCREEN_WIDTH      240
#define SCREEN_HEIGHT     400

#define BUTTON_LEFT       (1u << 5)
#define BUTTON_RIGHT      (1u << 4)

void *memset(void *dest, int val, size_t count)
{
    uint8_t *ptr = (uint8_t *)dest;
    while (count--)
        *ptr++ = (uint8_t)val;
    return dest;
}

void memset32(void *dest, uint32_t val, uint32_t words)
{
    uint32_t *p = (uint32_t *)dest;
    while (words--)
        *p++ = val;
}

static void delay(int count)
{
    for (volatile int i = 0; i < count; i++)
        __asm__ volatile("nop");
}

static void draw_rect(int x, int y, int w, int h, uint32_t color)
{
    uint32_t *vram = (uint32_t *)TOP_SCREEN_PADDR;
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            int px = x + j;
            int py = y + i;
            if (px >= 0 && px < SCREEN_WIDTH && py >= 0 && py < SCREEN_HEIGHT)
                vram[py * SCREEN_WIDTH + px] = color;
        }
    }
}

int main(void)
{
    REG_PDN_CLKEN0 |= (1u << 11);
    delay(8000);

    uint32_t *fb = (uint32_t *)TOP_SCREEN_PADDR;
    uint32_t total = (uint32_t)SCREEN_WIDTH * (uint32_t)SCREEN_HEIGHT;

    int bar_x = 100;
    const int bar_y = 350;
    const int bar_w = 40;
    const int bar_h = 10;

    while (1) {
        memset32(fb, 0x00000000u, total);

        draw_rect(bar_x, bar_y, bar_w, bar_h, 0x001F3F7Fu);
        draw_rect(0, 0, 8, 8, 0x000000FFu);
        draw_rect(SCREEN_WIDTH - 8, 0, 8, 8, 0x0000FF00u);

        uint32_t kDown = ~REG_PAD_HID;
        if (kDown & BUTTON_LEFT) {
            bar_x -= 4;
            if (bar_x < 0)
                bar_x = 0;
        }
        if (kDown & BUTTON_RIGHT) {
            bar_x += 4;
            if (bar_x > SCREEN_WIDTH - bar_w)
                bar_x = SCREEN_WIDTH - bar_w;
        }

        delay(40000);
    }

    return 0;
}
