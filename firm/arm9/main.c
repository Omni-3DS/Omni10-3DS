/*
 * Omni10-3DS ARM9 payload v0.3.8
 * left-aligned UI | internet bar | connection tester | power/reboot
 */

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define REG_PAD_HID    (*(volatile uint32_t *)0x10146000)
#define PDN_MPCORE_CFG (*(volatile uint8_t  *)0x10140FFC)

#define SCREEN_W 400
#define SCREEN_H 240
#define BOT_W    320

#define FB_TOP0 ((volatile uint8_t *)0x18300000)
#define FB_TOP1 ((volatile uint8_t *)0x18400000)
#define FB_BOT0 ((volatile uint8_t *)0x18346500)
#define FB_BOT1 ((volatile uint8_t *)0x18446500)

#define BTN_A      (1u << 0)
#define BTN_B      (1u << 1)
#define BTN_SELECT (1u << 2)
#define BTN_START  (1u << 3)
#define BTN_RIGHT  (1u << 4)
#define BTN_LEFT   (1u << 5)
#define BTN_UP     (1u << 6)
#define BTN_DOWN   (1u << 7)

/* 3DS FB: column-major, Y flipped. Logical x=0 = physical LEFT */
#define PIXEL_OFFSET(x, y)     (((x) * SCREEN_H) + (SCREEN_H - 1 - (y)))
#define PIXEL_OFFSET_BOT(x, y) (((x) * SCREEN_H) + (SCREEN_H - 1 - (y)))

#define I2C2_REGS_BASE 0x10144000
#define I2C_STOP       (1u << 0)
#define I2C_START      (1u << 1)
#define I2C_PAUSE      (1u << 2)
#define I2C_ACK        (1u << 4)
#define I2C_DIRE_READ  (1u << 5)
#define I2C_IRQ_ENABLE (1u << 6)
#define I2C_ENABLE     (1u << 7)
#define I2C_GET_ACK(r) (((r) >> 4) & 1u)
#define MCU_DEV_ADDR   0x4A

#define COL_BG_R 12
#define COL_BG_G 12
#define COL_BG_B 28
#define COL_HDR_R 16
#define COL_HDR_G 20
#define COL_HDR_B 48

typedef struct {
    volatile uint8_t  REG_I2C_DATA;
    volatile uint8_t  REG_I2C_CNT;
    volatile uint16_t REG_I2C_CNTEX;
    volatile uint16_t REG_I2C_SCL;
} I2cRegs;

struct fb {
    uint8_t *top_left;
    uint8_t *top_right;
    uint8_t *bottom;
};

static volatile uint8_t *g_top[2];
static volatile uint8_t *g_bot[2];
static int g_n_top;
static int g_n_bot;

enum { LANG_EN = 0, LANG_DE = 1 };
static int g_lang = LANG_EN;

/* WiFi status cache: -1 unknown, 0 off/disabled, 1 on */
static int g_wifi_status = -1;

static const char *L(const char *en, const char *de)
{
    return g_lang == LANG_DE ? de : en;
}

void *memset(void *dest, int val, size_t count)
{
    uint8_t *p = (uint8_t *)dest;
    while (count--) *p++ = (uint8_t)val;
    return dest;
}

size_t strlen(const char *s)
{
    size_t n = 0;
    while (s[n]) n++;
    return n;
}

static int strncmp_s(const char *a, const char *b, int n)
{
    for (int i = 0; i < n; i++) {
        if (a[i] != b[i]) return (unsigned char)a[i] - (unsigned char)b[i];
        if (!a[i]) return 0;
    }
    return 0;
}

static void i2c_wait(I2cRegs *regs)
{
    while (regs->REG_I2C_CNT & I2C_ENABLE) ;
}

static void i2c_init(void)
{
    I2cRegs *regs = (I2cRegs *)I2C2_REGS_BASE;
    i2c_wait(regs);
    regs->REG_I2C_CNTEX = 2;
    regs->REG_I2C_SCL = 1280;
}

static bool i2c_write_mcu(uint8_t reg, uint8_t data)
{
    I2cRegs *regs = (I2cRegs *)I2C2_REGS_BASE;
    for (int tries = 0; tries < 8; tries++) {
        i2c_wait(regs);
        regs->REG_I2C_DATA = MCU_DEV_ADDR;
        regs->REG_I2C_CNT = I2C_ENABLE | I2C_IRQ_ENABLE | I2C_START;
        i2c_wait(regs);
        if (!I2C_GET_ACK(regs->REG_I2C_CNT)) {
            regs->REG_I2C_CNT = I2C_ENABLE | I2C_IRQ_ENABLE | I2C_PAUSE | I2C_STOP;
            continue;
        }
        regs->REG_I2C_DATA = reg;
        regs->REG_I2C_CNT = I2C_ENABLE | I2C_IRQ_ENABLE;
        i2c_wait(regs);
        if (!I2C_GET_ACK(regs->REG_I2C_CNT)) {
            regs->REG_I2C_CNT = I2C_ENABLE | I2C_IRQ_ENABLE | I2C_PAUSE | I2C_STOP;
            continue;
        }
        regs->REG_I2C_DATA = data;
        regs->REG_I2C_CNT = I2C_ENABLE | I2C_IRQ_ENABLE | I2C_STOP;
        i2c_wait(regs);
        return true;
    }
    return false;
}

/* Read one byte from MCU register */
static bool i2c_read_mcu(uint8_t reg, uint8_t *out)
{
    I2cRegs *regs = (I2cRegs *)I2C2_REGS_BASE;
    for (int tries = 0; tries < 8; tries++) {
        i2c_wait(regs);
        regs->REG_I2C_DATA = MCU_DEV_ADDR;
        regs->REG_I2C_CNT = I2C_ENABLE | I2C_IRQ_ENABLE | I2C_START;
        i2c_wait(regs);
        if (!I2C_GET_ACK(regs->REG_I2C_CNT)) {
            regs->REG_I2C_CNT = I2C_ENABLE | I2C_IRQ_ENABLE | I2C_PAUSE | I2C_STOP;
            continue;
        }
        regs->REG_I2C_DATA = reg;
        regs->REG_I2C_CNT = I2C_ENABLE | I2C_IRQ_ENABLE;
        i2c_wait(regs);
        if (!I2C_GET_ACK(regs->REG_I2C_CNT)) {
            regs->REG_I2C_CNT = I2C_ENABLE | I2C_IRQ_ENABLE | I2C_PAUSE | I2C_STOP;
            continue;
        }
        regs->REG_I2C_DATA = (uint8_t)(MCU_DEV_ADDR | 1);
        regs->REG_I2C_CNT = I2C_ENABLE | I2C_IRQ_ENABLE | I2C_START;
        i2c_wait(regs);
        if (!I2C_GET_ACK(regs->REG_I2C_CNT)) {
            regs->REG_I2C_CNT = I2C_ENABLE | I2C_IRQ_ENABLE | I2C_PAUSE | I2C_STOP;
            continue;
        }
        regs->REG_I2C_CNT = I2C_ENABLE | I2C_IRQ_ENABLE | I2C_DIRE_READ | I2C_STOP;
        i2c_wait(regs);
        *out = regs->REG_I2C_DATA;
        return true;
    }
    return false;
}

static void power_off(void)
{
    i2c_init();
    i2c_write_mcu(0x22, 1u << 0);
    for (volatile int i = 0; i < 300000; i++) __asm__ volatile("nop");
    i2c_write_mcu(0x20, 1u << 0);
    while (1) ;
}

static void reboot(void)
{
    i2c_init();
    i2c_write_mcu(0x22, 1u << 0);
    for (volatile int i = 0; i < 300000; i++) __asm__ volatile("nop");
    i2c_write_mcu(0x20, 1u << 2);
    while (1) ;
}

/* WiFi status via MCU:
 * 0x2A = WiFi LED (non-zero = on)
 * 0x61 byte0 bit0 = WirelessDisabled (1 = disabled)
 */
static int wifi_probe(void)
{
    uint8_t led = 0, flags = 0;
    int ok_led = i2c_read_mcu(0x2A, &led);
    int ok_flg = i2c_read_mcu(0x61, &flags);
    if (!ok_led && !ok_flg) {
        g_wifi_status = -1;
        return -1;
    }
    if (ok_flg && (flags & 1u)) {
        g_wifi_status = 0;
        return 0;
    }
    if (ok_led && led != 0) {
        g_wifi_status = 1;
        return 1;
    }
    g_wifi_status = 0;
    return 0;
}

static void drain(void)
{
    uint32_t z = 0;
    __asm__ volatile("mcr p15, 0, %0, c7, c10, 4" ::"r"(z) : "memory");
}

static void delay(int n)
{
    for (volatile int i = 0; i < n; i++) __asm__ volatile("nop");
}
