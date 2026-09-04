/*
 * Omni10-3DS ARM9 payload v0.3.2
 * menu | lang EN/DE | .o10 runner | power/reboot confirm
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

/* Standard 3DS FB: column-major, y flipped */
#define PIXEL_OFFSET(x, y) (((x) * SCREEN_H) + (SCREEN_H - 1 - (y)))
#define PIXEL_OFFSET_BOT(x, y) (((x) * SCREEN_H) + (SCREEN_H - 1 - (y)))

#define I2C2_REGS_BASE 0x10144000
#define I2C_ENABLE     (1u << 7)
#define I2C_IRQ_ENABLE (1u << 6)
#define I2C_START_BIT  (1u << 5)
#define I2C_STOP       (1u << 4)
#define I2C_ERROR      (1u << 1)
#define I2C_DIRE_WRITE 0
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

static const char *L(const char *en, const char *de)
{
    return g_lang == LANG_DE ? de : en;
}

void *memset(void *dest, int val, size_t count)
{
    uint8_t *p = (uint8_t *)dest;
    while (count--)
        *p++ = (uint8_t)val;
    return dest;
}

size_t strlen(const char *s)
{
    size_t n = 0;
    while (s[n])
        n++;
    return n;
}

static int strncmp_s(const char *a, const char *b, int n)
{
    for (int i = 0; i < n; i++) {
        if (a[i] != b[i])
            return (unsigned char)a[i] - (unsigned char)b[i];
        if (!a[i])
            return 0;
    }
    return 0;
}

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

static void reboot(void)
{
    i2c_write_mcu(0x20, 1u << 2);
    while (1)
        ;
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

static const uint8_t font8[64][8] = {
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    {0x18,0x3C,0x3C,0x18,0x18,0x00,0x18,0x00},
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
    {0x3E,0x63,0x73,0x7B,0x6F,0x67,0x3E,0x00},
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
    {0x0C,0x1E,0x33,0x33,0x3F,0x33,0x33,0x00},
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

static void put_bot(int x, int y, uint8_t r, uint8_t g, uint8_t b)
{
    if (x < 0 || y < 0 || x >= BOT_W || y >= SCREEN_H)
        return;
    int idx = PIXEL_OFFSET_BOT(x, y) * 3;
    for (int i = 0; i < g_n_bot; i++) {
        if (!g_bot[i])
            continue;
        g_bot[i][idx + 0] = b;
        g_bot[i][idx + 1] = g;
        g_bot[i][idx + 2] = r;
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

static void clear_bot(uint8_t r, uint8_t g, uint8_t b)
{
    for (int y = 0; y < SCREEN_H; y++)
        for (int x = 0; x < BOT_W; x++)
            put_bot(x, y, r, g, b);
}

static void draw_char(int x, int y, char c, uint8_t r, uint8_t g, uint8_t b)
{
    unsigned char uc = (unsigned char)c;
    if (uc >= 'a' && uc <= 'z')
        uc = (unsigned char)(uc - 32);
    int idx = (int)uc - 0x20;
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

static void draw_char_bot(int x, int y, char c, uint8_t r, uint8_t g, uint8_t b)
{
    unsigned char uc = (unsigned char)c;
    if (uc >= 'a' && uc <= 'z')
        uc = (unsigned char)(uc - 32);
    int idx = (int)uc - 0x20;
    if (idx < 0 || idx >= 64)
        idx = 0;
    const uint8_t *glyph = font8[idx];
    for (int row = 0; row < 8; row++) {
        uint8_t bits = glyph[row];
        for (int col = 0; col < 8; col++) {
            if (bits & (0x80 >> col))
                put_bot(x + col, y + row, r, g, b);
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

static void draw_text_bot(int x, int y, const char *s, uint8_t r, uint8_t g, uint8_t b)
{
    while (*s) {
        draw_char_bot(x, y, *s, r, g, b);
        x += 8;
        s++;
    }
}

static void draw_text_centered(int y, const char *s, uint8_t r, uint8_t g, uint8_t b)
{
    int w = (int)strlen(s) * 8;
    draw_text((SCREEN_W - w) / 2, y, s, r, g, b);
}

static void draw_text_bot_centered(int y, const char *s, uint8_t r, uint8_t g, uint8_t b)
{
    int w = (int)strlen(s) * 8;
    draw_text_bot((BOT_W - w) / 2, y, s, r, g, b);
}

static uint32_t pad_raw(void)
{
    return ~REG_PAD_HID;
}

static uint32_t wait_key(void)
{
    uint32_t prev = pad_raw();
    delay(35000);
    while (1) {
        uint32_t k = pad_raw();
        uint32_t pressed = k & ~prev;
        if (pressed)
            return pressed;
        prev = k;
        if ((k & BTN_START) && (k & BTN_SELECT))
            power_off();
        delay(1200);
    }
}

static int is_new3ds(void)
{
    return (PDN_MPCORE_CFG & 2) != 0;
}

static void draw_header(void)
{
    fill_rect(0, 0, SCREEN_W, 28, COL_HDR_R, COL_HDR_G, COL_HDR_B);
    draw_text_centered(10, "OMNI10", 0, 220, 255);
    fill_rect(0, 28, SCREEN_W, 2, 0, 180, 220);
}

static void draw_footer(const char *hint)
{
    fill_rect(0, SCREEN_H - 22, SCREEN_W, 22, COL_HDR_R, COL_HDR_G, COL_HDR_B);
    draw_text_centered(SCREEN_H - 15, hint, 180, 190, 210);
}

static void draw_bot_help(const char *l1, const char *l2)
{
    clear_bot(COL_BG_R, COL_BG_G, COL_BG_B);
    draw_text_bot_centered(40, "OMNI10", 0, 180, 220);
    if (l1)
        draw_text_bot_centered(100, l1, 200, 200, 210);
    if (l2)
        draw_text_bot_centered(120, l2, 140, 150, 170);
    draw_text_bot_centered(200, L("START+SELECT = OFF", "START+SELECT = AUS"), 120, 130, 150);
}

static int confirm(const char *title, const char *msg)
{
    clear_top(COL_BG_R, COL_BG_G, COL_BG_B);
    draw_header();
    draw_text_centered(70, title, 255, 200, 80);
    draw_text_centered(110, msg, 200, 200, 210);
    draw_text_centered(150, L("A = YES   B = NO", "A = JA   B = NEIN"), 160, 180, 200);
    draw_footer(L("CONFIRM", "BESTAETIGEN"));
    draw_bot_help(L("Confirm action", "Aktion bestaetigen"), NULL);
    drain();

    while (1) {
        uint32_t k = wait_key();
        if (k & BTN_A)
            return 1;
        if (k & BTN_B)
            return 0;
    }
}

static void screen_about(void)
{
    clear_top(COL_BG_R, COL_BG_G, COL_BG_B);
    draw_header();
    draw_text_centered(55, "OMNI10-3DS", 255, 255, 255);
    draw_text_centered(75, L("CUSTOM FIRM PAYLOAD", "EIGENES FIRM PAYLOAD"), 140, 200, 230);
    draw_text_centered(105, "VERSION 0.3.2", 160, 160, 180);
    draw_text_centered(130, ".O10  |  LUA  |  FTP", 100, 180, 210);
    draw_text_centered(155, "GITHUB.COM/OMNI-3DS", 90, 140, 180);
    draw_text_centered(185, L("FULL ACCESS. NO LIMITS.", "VOLLER ZUGRIFF. KEINE LIMITS."), 0, 200, 180);
    draw_footer(L("B = BACK", "B = ZURUECK"));
    draw_bot_help(L("About Omni10", "Ueber Omni10"), L("B to go back", "B = zurueck"));
    drain();

    while (1) {
        if (wait_key() & BTN_B)
            return;
    }
}

static void screen_sysinfo(void)
{
    clear_top(COL_BG_R, COL_BG_G, COL_BG_B);
    draw_header();
    draw_text_centered(40, L("SYSTEM INFO", "SYSTEMINFO"), 200, 220, 255);

    draw_text(40, 70, L("CONSOLE:", "KONSOLE:"), 160, 160, 180);
    draw_text(160, 70, is_new3ds() ? "NEW 3DS FAMILY" : "OLD 3DS FAMILY", 0, 255, 180);

    draw_text(40, 90, "ENTRY:", 160, 160, 180);
    draw_text(160, 90, "0x08000040", 200, 200, 220);

    draw_text(40, 110, "FB:", 160, 160, 180);
    draw_text(160, 110, "RGB888", 200, 200, 220);

    draw_text(40, 130, "LANG:", 160, 160, 180);
    draw_text(160, 130, g_lang == LANG_DE ? "DE" : "EN", 200, 200, 220);

    draw_text(40, 150, "STATUS:", 160, 160, 180);
    draw_text(160, 150, L("RUNNING", "LAEUFT"), 0, 255, 120);

    draw_footer(L("B = BACK", "B = ZURUECK"));
    draw_bot_help(L("Hardware details", "Hardwaredetails"), NULL);
    drain();

    while (1) {
        if (wait_key() & BTN_B)
            return;
    }
}

static void screen_settings(void)
{
    int sel = 0;
    while (1) {
        clear_top(COL_BG_R, COL_BG_G, COL_BG_B);
        draw_header();
        draw_text_centered(40, L("SETTINGS", "EINSTELLUNGEN"), 200, 220, 255);

        const char *items[2];
        items[0] = g_lang == LANG_DE ? "SPRACHE: DEUTSCH" : "LANGUAGE: ENGLISH";
        items[1] = L("BACK", "ZURUECK");

        for (int i = 0; i < 2; i++) {
            int y = 90 + i * 28;
            if (i == sel) {
                fill_rect(40, y - 4, SCREEN_W - 80, 18, 0, 70, 110);
                draw_text(56, y, items[i], 255, 255, 120);
            } else {
                draw_text(56, y, items[i], 190, 195, 210);
            }
        }

        draw_footer(L("A SELECT  |  B BACK", "A WAEHLEN  |  B ZURUECK"));
        draw_bot_help(L("Change language", "Sprache aendern"), L("A toggle  B back", "A umschalten  B zurueck"));
        drain();

        uint32_t k = wait_key();
        if (k & BTN_B)
            return;
        if (k & BTN_UP || k & BTN_DOWN)
            sel = 1 - sel;
        if (k & BTN_A) {
            if (sel == 0)
                g_lang = 1 - g_lang;
            else
                return;
        }
    }
}

static const char *demo_hello =
    "PRINT HELLO FROM O10\n"
    "WAIT\n"
    "PRINT OMNI10 RULES\n"
    "WAIT\n"
    "PRINT DONE\n";

static const char *demo_info =
    "PRINT SYSTEM CHECK\n"
    "WAIT\n"
    "PRINT PAYLOAD OK\n"
    "WAIT\n"
    "PRINT END\n";

static void o10_run(const char *src)
{
    char line[48];
    int li = 0;
    int log_y = 60;

    clear_top(COL_BG_R, COL_BG_G, COL_BG_B);
    draw_header();
    draw_text_centered(40, L("O10 RUNNER", "O10 AUSFUEHRUNG"), 0, 220, 255);
    draw_bot_help(L("Running script...", "Skript laeuft..."), L("B to abort", "B = abbrechen"));
    drain();

    while (*src) {
        if (pad_raw() & BTN_B)
            break;

        if (*src == '\n' || *src == 0) {
            line[li] = 0;
            if (li > 0) {
                if (strncmp_s(line, "PRINT ", 6) == 0) {
                    draw_text(40, log_y, line + 6, 200, 255, 200);
                    log_y += 14;
                    if (log_y > 200)
                        log_y = 60;
                    drain();
                } else if (strncmp_s(line, "WAIT", 4) == 0) {
                    delay(600000);
                } else if (strncmp_s(line, "POWEROFF", 8) == 0) {
                    power_off();
                } else if (strncmp_s(line, "REBOOT", 6) == 0) {
                    reboot();
                }
            }
            li = 0;
            if (*src == 0)
                break;
            src++;
            continue;
        }
        if (li < 46)
            line[li++] = *src;
        src++;
    }

    draw_text_centered(210, L("A / B = BACK", "A / B = ZURUECK"), 160, 160, 180);
    drain();
    while (1) {
        uint32_t k = wait_key();
        if (k & (BTN_A | BTN_B))
            return;
    }
}

static void screen_scripts(void)
{
    int sel = 0;
    while (1) {
        clear_top(COL_BG_R, COL_BG_G, COL_BG_B);
        draw_header();
        draw_text_centered(38, L("SCRIPTS .O10", "SKRIPTE .O10"), 200, 220, 255);

        const char *items[] = {
            "HELLO.O10",
            "SYSCHECK.O10",
            L("BACK", "ZURUECK"),
        };
        const int n = 3;

        for (int i = 0; i < n; i++) {
            int y = 70 + i * 24;
            if (i == sel) {
                fill_rect(40, y - 3, SCREEN_W - 80, 18, 0, 70, 110);
                draw_text(56, y, items[i], 255, 255, 120);
            } else {
                draw_text(56, y, items[i], 190, 195, 210);
            }
        }

        draw_text_centered(170, L("BUILT-IN DEMOS", "EINGEBAUTE DEMOS"), 120, 140, 160);
        draw_footer(L("A RUN  |  B BACK", "A START  |  B ZURUECK"));
        draw_bot_help(L("Run .o10 script", ".o10 Skript starten"), NULL);
        drain();

        uint32_t k = wait_key();
        if (k & BTN_B)
            return;
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
                o10_run(demo_hello);
            else if (sel == 1)
                o10_run(demo_info);
            else
                return;
        }
    }
}

static void screen_placeholder(const char *title, const char *l1, const char *l2)
{
    clear_top(COL_BG_R, COL_BG_G, COL_BG_B);
    draw_header();
    draw_text_centered(80, title, 255, 220, 100);
    draw_text_centered(120, l1, 180, 180, 200);
    draw_text_centered(140, l2, 140, 140, 160);
    draw_footer(L("B = BACK", "B = ZURUECK"));
    draw_bot_help(title, L("Coming soon", "Kommt bald"));
    drain();
    while (1) {
        if (wait_key() & BTN_B)
            return;
    }
}

static void screen_menu(void)
{
    int sel = 0;
    while (1) {
        const char *items[] = {
            L("ABOUT", "INFO"),
            L("SYSTEM INFO", "SYSTEMINFO"),
            L("SCRIPTS .O10", "SKRIPTE .O10"),
            L("SETTINGS", "EINSTELLUNGEN"),
            L("FILE BROWSER (SOON)", "DATEIBROWSER (BALD)"),
            L("FTP SERVER (SOON)", "FTP-SERVER (BALD)"),
            L("REBOOT", "NEUSTART"),
            L("POWER OFF", "AUSSCHALTEN"),
        };
        const int n = 8;

        clear_top(COL_BG_R, COL_BG_G, COL_BG_B);
        draw_header();
        draw_text_centered(36, L("MAIN MENU", "HAUPTMENUE"), 180, 210, 255);

        for (int i = 0; i < n; i++) {
            int y = 52 + i * 18;
            if (i == sel) {
                fill_rect(32, y - 2, SCREEN_W - 64, 15, 0, 70, 110);
                draw_text(48, y, items[i], 255, 255, 120);
            } else {
                draw_text(48, y, items[i], 190, 195, 210);
            }
        }

        draw_footer(L("A SELECT | START+SELECT OFF", "A WAEHLEN | START+SELECT AUS"));
        draw_bot_help(L("Navigate with D-Pad", "Mit Steuerkreuz navigieren"),
                      L("A select  B back", "A waehlen  B zurueck"));
        drain();

        uint32_t k = wait_key();
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
            switch (sel) {
            case 0: screen_about(); break;
            case 1: screen_sysinfo(); break;
            case 2: screen_scripts(); break;
            case 3: screen_settings(); break;
            case 4:
                screen_placeholder(L("FILE BROWSER", "DATEIBROWSER"),
                                   L("SD / NAND ACCESS", "SD / NAND ZUGRIFF"),
                                   L("COMING SOON", "KOMMT BALD"));
                break;
            case 5:
                screen_placeholder(L("FTP SERVER", "FTP-SERVER"),
                                   L("REAL FTP OVER WIFI", "ECHTES FTP UEBER WIFI"),
                                   L("COMING SOON", "KOMMT BALD"));
                break;
            case 6:
                if (confirm(L("REBOOT?", "NEUSTART?"), L("Restart the console", "Konsole neu starten")))
                    reboot();
                break;
            case 7:
                if (confirm(L("POWER OFF?", "AUSSCHALTEN?"), L("Turn console off", "Konsole ausschalten")))
                    power_off();
                break;
            }
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

    clear_top(8, 8, 24);
    clear_bot(8, 8, 24);
    draw_text_centered(90, "OMNI10", 0, 220, 255);
    draw_text_centered(112, "BOOTING...", 120, 150, 180);
    draw_text_centered(200, "V0.3.2", 80, 100, 120);
    draw_text_bot_centered(110, "FULL ACCESS", 0, 180, 200);
    drain();
    delay(700000);

    screen_menu();
    return 0;
}
