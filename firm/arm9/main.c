/*
 * Omni10-3DS – ARM9 payload
 * RGB888 Luma FB, PIXEL_OFFSET, 8x8 font, main menu
 */

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define REG_PAD_HID (*(volatile uint32_t *)0x10146000)

#define SCREEN_W 400
#define SCREEN_H 240

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

#define PIXEL_OFFSET(x, y) (((x) * SCREEN_H) + (SCREEN_H - (y) - 1))

#define I2C2_REGS_BASE 0x10144000
#define I2C_ENABLE     (1u << 7)
#define I2C_IRQ_ENABLE (1u << 6)
#define I2C_START_BIT  (1u << 5)
#define I2C_STOP       (1u << 4)
#define I2C_ERROR      (1u << 1)
#define I2C_DIRE_WRITE 0
#define I2C_GET_ACK(r) (((r) >> 4) & 1u)
#define MCU_DEV_ADDR   0x4A

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

/* ---- libc stubs ---- */
void *memset(void *dest, int val, size_t count)
{
    uint8_t *p = (uint8_t *)dest;
    while (count--)
        *p++ = (uint8_t)val;
    return dest;
}

static int strlen_s(const char *s)
{
    int n = 0;
    while (s[n])
        n++;
    return n;
}

/* ---- I2C / power ---- */
static void i2c_wait(I2cRegs *regs)
{
    while (regs->REG_I2C_CNT & I2C_ENABLE)
        ;
}

static bool i2c_write_mcu(uint8_t reg, uint8_t data)
{
    I2cRegs *regs = (I2cRegs *)I2C2_REGS_BASE;
    for (int tries = 0; tries < 8; tries++) {
        i2c_wait(regs);
        regs->REG_I2C_DATA = MCU_DEV_ADDR;
        regs->REG_I2C_CNT = I2C_ENABLE | I2C_IRQ_ENABLE | I2C_START_BIT;
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
        if (!I2C_GET_ACK(regs->REG_I2C_CNT))
            continue;
        return true;
    }
    return false;
}

static void power_off(void)
{
    i2c_write_mcu(0x20, 1);
    while (1)
        ;
}

/* ---- timing / cache ---- */
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

/* ---- 8x8 font (ASCII 0x20-0x5F) ---- */
static const uint8_t font8[64][8] = {
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, /* space */
    {0x18,0x3C,0x3C,0x18,0x18,0x00,0x18,0x00}, /* ! */
    {0x36,0x36,0x00,0x00,0x00,0x00,0x00,0x00},
    {0x36,0x36,0x7F,0x36,0x7F,0x36,0x36,0x00},
    {0x0C,0x3E,0x03,0x1E,0x30,0x1F,0x0C,0x00},
    {0x00,0x63,0x33,0x18,0x0C,0x66,0x63,0x00},
    {0x1C,0x36,0x1C,0x6E,0x3B,0x33,0x6E,0x00},
    {0x06,0x06,0x03,0x00,0x00,0x00,0x00,0x00},
    {0x18,0x0C,0x06,0x06,0x06,0x0C,0x18,0x00},
    {0x06,0x0C,0x18,0x18,0x18,0x0C,0x06,0x00},
    {0x00,0x66,0x3C,0xFF,0x3C,0x66,0x00,0x00},
    {0x00,0x0C,0x0C,0x3F,0x0C,0x0C,0x00,0x00},
    {0x00,0x00,0x00,0x00,0x00,0x0C,0x0C,0x06},
    {0x00,0x00,0x00,0x3F,0x00,0x00,0x00,0x00},
    {0x00,0x00,0x00,0x00,0x00,0x0C,0x0C,0x00},
    {0x60,0x30,0x18,0x0C,0x06,0x03,0x01,0x00},
    {0x3E,0x63,0x73,0x7B,0x6F,0x67,0x3E,0x00}, /* 0 */
    {0x0C,0x0E,0x0C,0x0C,0x0C,0x0C,0x3F,0x00},
    {0x1E,0x33,0x30,0x1C,0x06,0x33,0x3F,0x00},
    {0x1E,0x33,0x30,0x1C,0x30,0x33,0x1E,0x00},
    {0x38,0x3C,0x36,0x33,0x7F,0x30,0x78,0x00},
    {0x3F,0x03,0x1F,0x30,0x30,0x33,0x1E,0x00},
    {0x1C,0x06,0x03,0x1F,0x33,0x33,0x1E,0x00},
    {0x3F,0x33,0x30,0x18,0x0C,0x0C,0x0C,0x00},
    {0x1E,0x33,0x33,0x1E,0x33,0x33,0x1E,0x00},
    {0x1E,0x33,0x33,0x3E,0x30,0x18,0x0E,0x00},
    {0x00,0x0C,0x0C,0x00,0x00,0x0C,0x0C,0x00},
    {0x00,0x0C,0x0C,0x00,0x00,0x0C,0x0C,0x06},
    {0x18,0x0C,0x06,0x03,0x06,0x0C,0x18,0x00},
    {0x00,0x00,0x3F,0x00,0x00,0x3F,0x00,0x00},
    {0x06,0x0C,0x18,0x30,0x18,0x0C,0x06,0x00},
    {0x1E,0x33,0x30,0x18,0x0C,0x00,0x0C,0x00},
    {0x3E,0x63,0x7B,0x7B,0x7B,0x03,0x1E,0x00},
    {0x0C,0x1E,0x33,0x33,0x3F,0x33,0x33,0x00}, /* A */
    {0x3F,0x66,0x66,0x3E,0x66,0x66,0x3F,0x00},
    {0x3C,0x66,0x03,0x03,0x03,0x66,0x3C,0x00},
    {0x1F,0x36,0x66,0x66,0x66,0x36,0x1F,0x00},
    {0x7F,0x46,0x16,0x1E,0x16,0x46,0x7F,0x00},
    {0x7F,0x46,0x16,0x1E,0x16,0x06,0x0F,0x00},
    {0x3C,0x66,0x03,0x03,0x73,0x66,0x7C,0x00},
    {0x33,0x33,0x33,0x3F,0x33,0x33,0x33,0x00},
    {0x1E,0x0C,0x0C,0x0C,0x0C,0x0C,0x1E,0x00},
    {0x78,0x30,0x30,0x30,0x33,0x33,0x1E,0x00},
    {0x67,0x66,0x36,0x1E,0x36,0x66,0x67,0x00},
    {0x0F,0x06,0x06,0x06,0x46,0x66,0x7F,0x00},
    {0x63,0x77,0x7F,0x6B,0x63,0x63,0x63,0x00},
    {0x63,0x67,0x6F,0x7B,0x73,0x63,0x63,0x00},
    {0x1C,0x36,0x63,0x63,0x63,0x36,0x1C,0x00},
    {0x3F,0x66,0x66,0x3E,0x06,0x06,0x0F,0x00},
    {0x1E,0x33,0x33,0x33,0x3B,0x1E,0x38,0x00},
    {0x3F,0x66,0x66,0x3E,0x36,0x66,0x67,0x00},
    {0x1E,0x33,0x07,0x0E,0x38,0x33,0x1E,0x00},
    {0x3F,0x2D,0x0C,0x0C,0x0C,0x0C,0x1E,0x00},
    {0x33,0x33,0x33,0x33,0x33,0x33,0x3F,0x00},
    {0x33,0x33,0x33,0x33,0x33,0x1E,0x0C,0x00},
    {0x63,0x63,0x63,0x6B,0x7F,0x77,0x63,0x00},
    {0x63,0x63,0x36,0x1C,0x1C,0x36,0x63,0x00},
    {0x33,0x33,0x33,0x1E,0x0C,0x0C,0x1E,0x00},
    {0x7F,0x63,0x31,0x18,0x4C,0x66,0x7F,0x00},
    {0x1E,0x06,0x06,0x06,0x06,0x06,0x1E,0x00},
    {0x03,0x06,0x0C,0x18,0x30,0x60,0x40,0x00},
    {0x1E,0x18,0x18,0x18,0x18,0x18,0x1E,0x00},
    {0x08,0x1C,0x36,0x63,0x00,0x00,0x00,0x00},
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF},
};

/* ---- draw ---- */
static void put(int x, int y, uint8_t r, uint8_t g, uint8_t b)
{
    if (x < 0 || y < 0 || x >= SCREEN_W || y >= SCREEN_H)
        return;
    int idx = PIXEL_OFFSET(x, y) * 3;
    for (int i = 0; i < g_n_top; i++) {
        if (!g_top[i])
            continue;
        g_top[i][idx + 0] = b;
        g_top[i][idx + 1] = g;
        g_top[i][idx + 2] = r;
    }
}

static void fill_rect(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b)
{
    for (int j = 0; j < h; j++)
        for (int i = 0; i < w; i++)
            put(x + i, y + j, r, g, b);
}

static void clear_top(uint8_t r, uint8_t g, uint8_t b)
{
    fill_rect(0, 0, SCREEN_W, SCREEN_H, r, g, b);
}

static void clear_bot(void)
{
    for (int t = 0; t < g_n_bot; t++) {
        if (!g_bot[t])
            continue;
        for (int i = 0; i < 320 * 240 * 3; i++)
            g_bot[t][i] = 0;
    }
}

static void draw_char(int x, int y, char c, uint8_t r, uint8_t g, uint8_t b)
{
    int idx = (int)(unsigned char)c - 0x20;
    if (idx < 0 || idx >= 64)
        idx = 0;
    const uint8_t *glyph = font8[idx];
    for (int row = 0; row < 8; row++) {
        uint8_t bits = glyph[row];
        for (int col = 0; col < 8; col++) {
            if (bits & (0x80 >> col))
                put(x + col, y + row, r, g, b);
        }
    }
}

static void draw_text(int x, int y, const char *s, uint8_t r, uint8_t g, uint8_t b)
{
    while (*s) {
        draw_char(x, y, *s, r, g, b);
        x += 8;
        s++;
    }
}

static void draw_text_centered(int y, const char *s, uint8_t r, uint8_t g, uint8_t b)
{
    int w = strlen_s(s) * 8;
    draw_text((SCREEN_W - w) / 2, y, s, r, g, b);
}

/* ---- input ---- */
static uint32_t pad_raw(void)
{
    return ~REG_PAD_HID;
}

static uint32_t wait_key(void)
{
    uint32_t prev = pad_raw();
    delay(50000);
    while (1) {
        uint32_t k = pad_raw();
        uint32_t pressed = k & ~prev;
        if (pressed)
            return pressed;
        prev = k;
        delay(2000);
    }
}

/* ---- screens ---- */
static void draw_header(void)
{
    fill_rect(0, 0, SCREEN_W, 28, 20, 20, 40);
    draw_text_centered(10, "OMNI10", 0, 220, 255);
    fill_rect(0, 28, SCREEN_W, 2, 0, 160, 200);
}

static void draw_footer(const char *hint)
{
    fill_rect(0, SCREEN_H - 20, SCREEN_W, 20, 20, 20, 40);
    draw_text_centered(SCREEN_H - 14, hint, 180, 180, 200);
}

static void screen_about(void)
{
    clear_top(12, 12, 24);
    draw_header();
    draw_text_centered(60, "Omni10-3DS", 255, 255, 255);
    draw_text_centered(80, "Custom FIRM Payload", 160, 200, 220);
    draw_text_centered(110, "Version 0.1.0", 140, 140, 160);
    draw_text_centered(140, ".o10 scripts  |  FTP", 120, 180, 200);
    draw_text_centered(170, "github.com/Omni-3DS", 100, 140, 180);
    draw_footer("B = back");
    clear_bot();
    drain();

    while (1) {
        uint32_t k = wait_key();
        if (k & (BTN_B | BTN_START))
            return;
        if ((k & BTN_START) && (k & BTN_SELECT))
            power_off();
    }
}

static void screen_menu(void)
{
    static const char *items[] = {
        "About Omni10",
        "File Browser  (soon)",
        "Scripts .o10  (soon)",
        "FTP Server    (soon)",
        "Power Off",
    };
    const int n = 5;
    int sel = 0;

    while (1) {
        clear_top(12, 12, 24);
        draw_header();
        draw_text_centered(40, "Main Menu", 200, 220, 255);

        for (int i = 0; i < n; i++) {
            int y = 70 + i * 22;
            if (i == sel) {
                fill_rect(40, y - 2, SCREEN_W - 80, 18, 0, 80, 120);
                draw_text(56, y, items[i], 255, 255, 100);
            } else {
                draw_text(56, y, items[i], 200, 200, 220);
            }
        }

        draw_footer("A select  |  START+SELECT power");
        clear_bot();
        drain();

        uint32_t k = wait_key();
        if ((k & BTN_START) && (k & BTN_SELECT))
            power_off();
        if (k & BTN_UP) {
            sel--;
            if (sel < 0)
                sel = n - 1;
        }
        if (k & BTN_DOWN) {
            sel++;
            if (sel >= n)
                sel = 0;
        }
        if (k & BTN_A) {
            if (sel == 0)
                screen_about();
            else if (sel == 4)
                power_off();
            /* others: soon */
        }
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
            g_top[g_n_top++] = (volatile uint8_t *)fbs[0].top_left;
        if (fbs[1].top_left)
            g_top[g_n_top++] = (volatile uint8_t *)fbs[1].top_left;
        if (fbs[0].bottom)
            g_bot[g_n_bot++] = (volatile uint8_t *)fbs[0].bottom;
        if (fbs[1].bottom)
            g_bot[g_n_bot++] = (volatile uint8_t *)fbs[1].bottom;
        if (g_n_top == 0) {
            g_top[0] = FB_TOP0;
            g_top[1] = FB_TOP1;
            g_n_top = 2;
        }
    }

    /* brief splash */
    clear_top(8, 8, 20);
    draw_text_centered(100, "OMNI10", 0, 220, 255);
    draw_text_centered(120, "booting...", 120, 140, 180);
    clear_bot();
    drain();
    delay(800000);

    screen_menu();
    return 0;
}
