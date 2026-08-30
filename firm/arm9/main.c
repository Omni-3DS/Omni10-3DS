/*
 * Omni10-3DS – minimal bootable ARM9 payload
 * Build pattern taken from SlabyLol/3DSFirm (your working firm)
 */

#include <stdint.h>
#include <stddef.h>

#define REG_PDN_CLKEN0   (*(volatile uint32_t *)0x10141000)
#define TOP_SCREEN_PADDR 0x18000000

/* Native framebuffer orientation used in 3DSFirm */
#define SCREEN_WIDTH  240
#define SCREEN_HEIGHT 400

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

void delay(int count)
{
    for (volatile int i = 0; i < count; i++)
        __asm__("nop");
}

void draw_rect(int x, int y, int w, int h, uint32_t color)
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
    delay(5000);

    uint32_t *fb = (uint32_t *)TOP_SCREEN_PADDR;
    uint32_t total = SCREEN_WIDTH * SCREEN_HEIGHT;

    /* Black screen + simple Omni10 marker bar */
    memset32(fb, 0x00000000, total);
    draw_rect(20, 180, 200, 40, 0x001F3F7F); /* blue bar */

    while (1) {
        delay(100000);
    }

    return 0;
}
