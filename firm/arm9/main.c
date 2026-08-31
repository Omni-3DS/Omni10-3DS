/*
 * Omni10 – draw + START+SELECT power off
 */

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define REG_PAD_HID (*(volatile uint32_t *)0x10146000)

#define SCREEN_W 400
#define SCREEN_H 240

#define FB_TOP0 ((volatile uint16_t *)0x18300000)
#define FB_TOP1 ((volatile uint16_t *)0x18400000)
#define FB_BOT0 ((volatile uint16_t *)0x18346500)
#define FB_BOT1 ((volatile uint16_t *)0x18446500)

#define BTN_A      (1u << 0)
#define BTN_B      (1u << 1)
#define BTN_SELECT (1u << 2)
#define BTN_START  (1u << 3)
#define BTN_RIGHT  (1u << 4)
#define BTN_LEFT   (1u << 5)

#define RGB565(r, g, b) ((uint16_t)(((r) & 0x1F) << 11 | ((g) & 0x3F) << 5 | ((b) & 0x1F)))
#define PIXEL_OFFSET(x, y) (((x) * SCREEN_H) + (SCREEN_H - (y) - 1))

/* ---- minimal I2C (MCU power off) ---- */
#define I2C2_REGS_BASE 0x10144000

#define I2C_ENABLE     (1u << 7)
#define I2C_IRQ_ENABLE (1u << 6)
#define I2C_START      (1u << 5)
#define I2C_STOP       (1u << 4)
#define I2C_ERROR      (1u << 1)
#define I2C_ACK        (1u << 2)
#define I2C_DIRE_WRITE 0
#define I2C_GET_ACK(r) (((r) >> 4) & 1u)

typedef struct {
    volatile uint8_t  REG_I2C_DATA;
    volatile uint8_t  REG_I2C_CNT;
    volatile uint16_t REG_I2C_CNTEX;
    volatile uint16_t REG_I2C_SCL;
} I2cRegs;

/* MCU = device 3, bus 1, addr 0x4A */
#define MCU_DEV_ADDR 0x4A

static void i2c_wait(I2cRegs *regs)
{
    while (regs->REG_I2C_CNT & I2C_ENABLE)
        ;
}

static bool i2c_write_mcu(uint8_t reg, uint8_t data)
{
    I2cRegs *regs = (I2cRegs *)I2C2_REGS_BASE;
    int tries;

    for (tries = 0; tries < 8; tries++) {
        i2c_wait(regs);
        regs->REG_I2C_DATA = MCU_DEV_ADDR;
        regs->REG_I2C_CNT = I2C_ENABLE | I2C_IRQ_ENABLE | I2C_START;
        i2c_wait(regs);
        if (!I2C_GET_ACK(regs->REG_I2C_CNT)) {
            regs->REG_I2C_CNT = I2C_ENABLE | I2C_IRQ_ENABLE | I2C_ERROR | I2C_STOP;
            continue;
        }

        regs->REG_I2C_DATA = reg;
        regs->REG_I2C_CNT = I2C_ENABLE | I2C_IRQ_ENABLE | I2C_DIRE_WRITE;
        i2c_wait(regs);
        if (!I2C_GET_ACK(regs->REG_I2C_CNT)) {
            regs->REG_I2C_CNT = I2C_ENABLE | I2C_IRQ_ENABLE | I2C_ERROR | I2C_STOP;
            continue;
        }

        regs->REG_I2C_DATA = data;
        regs->REG_I2C_CNT = I2C_ENABLE | I2C_IRQ_ENABLE | I2C_DIRE_WRITE | I2C_STOP;
        i2c_wait(regs);
        if (!I2C_GET_ACK(regs->REG_I2C_CNT)) {
            regs->REG_I2C_CNT = I2C_ENABLE | I2C_IRQ_ENABLE | I2C_ERROR | I2C_STOP;
            continue;
        }
        return true;
    }
    return false;
}

static void power_off(void)
{
    /* MCU reg 0x20 bit0 = power off */
    i2c_write_mcu(0x20, 1);
    while (1)
        ;
}

/* ---- FB ---- */
struct fb {
    uint8_t *top_left;
    uint8_t *top_right;
    uint8_t *bottom;
};

static volatile uint16_t *g_top[2];
static volatile uint16_t *g_bot[2];
static int g_n_top;
static int g_n_bot;

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
    if (x < 0 || y < 0 || x >= SCREEN_W || y >= SCREEN_H)
        return;
    int idx = PIXEL_OFFSET(x, y);
    for (int i = 0; i < g_n_top; i++)
        if (g_top[i])
            g_top[i][idx] = c;
}

static void clear_top(uint16_t color)
{
    uint32_t c32 = ((uint32_t)color << 16) | color;
    int n = (SCREEN_W * SCREEN_H) / 2;
    for (int t = 0; t < g_n_top; t++) {
        if (!g_top[t])
            continue;
        volatile uint32_t *p = (volatile uint32_t *)g_top[t];
        for (int i = 0; i < n; i++)
            p[i] = c32;
    }
}

static void rect(int x, int y, int w, int h, uint16_t c)
{
    for (int j = 0; j < h; j++)
        for (int i = 0; i < w; i++)
            put(x + i, y + j, c);
}

static void clear_bot(uint16_t color)
{
    uint32_t c32 = ((uint32_t)color << 16) | color;
    int n = (240 * 320) / 2;
    for (int t = 0; t < g_n_bot; t++) {
        if (!g_bot[t])
            continue;
        volatile uint32_t *p = (volatile uint32_t *)g_bot[t];
        for (int i = 0; i < n; i++)
            p[i] = c32;
    }
}

int main(int argc, char **argv)
{
    g_top[0] = FB_TOP0;
    g_top[1] = FB_TOP1;
    g_bot[0] = FB_BOT0;
    g_bot[1] = FB_BOT1;
    g_n_top = 2;
    g_n_bot = 2;

    if (argc >= 2 && argv && argv[1]) {
        struct fb *fbs = (struct fb *)argv[1];
        g_n_top = 0;
        g_n_bot = 0;
        if (fbs[0].top_left)
            g_top[g_n_top++] = (volatile uint16_t *)fbs[0].top_left;
        if (fbs[1].top_left)
            g_top[g_n_top++] = (volatile uint16_t *)fbs[1].top_left;
        if (fbs[0].bottom)
            g_bot[g_n_bot++] = (volatile uint16_t *)fbs[0].bottom;
        if (fbs[1].bottom)
            g_bot[g_n_bot++] = (volatile uint16_t *)fbs[1].bottom;
        if (g_n_top == 0) {
            g_top[0] = FB_TOP0;
            g_top[1] = FB_TOP1;
            g_n_top = 2;
        }
    }

    const uint16_t BLK = RGB565(0, 0, 0);
    const uint16_t RED = RGB565(31, 0, 0);
    const uint16_t GRN = RGB565(0, 63, 0);
    const uint16_t BLU = RGB565(0, 0, 31);
    const uint16_t WHT = RGB565(31, 63, 31);
    const uint16_t CYN = RGB565(0, 55, 31);

    clear_top(BLK);
    rect(0, 0, 200, 120, RED);
    rect(200, 0, 200, 120, GRN);
    rect(0, 120, 200, 120, BLU);
    rect(200, 120, 200, 120, WHT);
    clear_bot(BLK);
    drain();

    int bar_x = 160;
    int prev_x = -1;
    const int bar_y = 105;
    const int bar_w = 80;
    const int bar_h = 28;

    while (1) {
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

        /* START + SELECT → power off */
        if ((k & BTN_START) && (k & BTN_SELECT)) {
            clear_top(BLK);
            clear_bot(BLK);
            drain();
            power_off();
        }

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
