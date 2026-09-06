/*
 * Omni10 ARM11 core — runs from AXI WRAM (0x1FF80000).
 * Shared-memory mailbox with ARM9 for WiFi/FTP/status.
 *
 * Note: full NWM/soc:u is not available inside a bare FIRM replace.
 * ARM11 here owns the protocol + state machine so dual-core is real;
 * TCP/SSID scan still needs either a kernel module path or homebrew soc.
 */

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;

#define O10_SHM_MAGIC  0x4F313041u /* 'O10A' */
#define O10_SHM_ADDR   0x1FF80E00u

enum {
    CMD_NOP = 0,
    CMD_PING = 1,
    CMD_WIFI_STATUS = 2,
    CMD_FTP_START = 3,
    CMD_FTP_STOP = 4,
    CMD_FTP_STATUS = 5,
    CMD_SET_LED = 6,
    CMD_GET_VERSION = 7
};

typedef struct {
    u32 magic;
    u32 arm11_alive;   /* 1 when ARM11 loop running */
    u32 cmd;           /* ARM9 writes */
    u32 arg0;
    u32 arg1;
    u32 status;        /* ARM11 response */
    u32 wifi_on;       /* 0/1/0xFF unknown */
    u32 ftp_on;
    u32 ftp_port;      /* 21 */
    u32 seq;           /* increments each handled cmd */
    u32 version;       /* 0x00090101 = 0.9.1 */
    u32 reserved[5];
} o10_shm_t;

static volatile o10_shm_t *shm(void)
{
    return (volatile o10_shm_t *)O10_SHM_ADDR;
}

static void delay(u32 n)
{
    while (n--) {
        __asm__ __volatile__("nop");
    }
}

static void handle_cmd(volatile o10_shm_t *s)
{
    u32 c = s->cmd;
    if (c == CMD_NOP)
        return;

    switch (c) {
    case CMD_PING:
        s->status = 0xA11Cu; /* 'A11' marker */
        break;
    case CMD_WIFI_STATUS:
        /* Soft status: reflect arg0 if ARM9 already probed MCU, else keep */
        if (s->arg0 <= 1)
            s->wifi_on = s->arg0;
        s->status = s->wifi_on;
        break;
    case CMD_FTP_START:
        s->ftp_on = 1;
        s->ftp_port = s->arg0 ? s->arg0 : 21;
        s->status = 1;
        break;
    case CMD_FTP_STOP:
        s->ftp_on = 0;
        s->status = 0;
        break;
    case CMD_FTP_STATUS:
        s->status = s->ftp_on;
        break;
    case CMD_SET_LED:
        /* LED is MCU on ARM9; ARM11 just acks */
        s->status = s->arg0 & 0xFF;
        break;
    case CMD_GET_VERSION:
        s->status = s->version;
        break;
    default:
        s->status = 0xFFFFFFFFu;
        break;
    }

    s->seq++;
    s->cmd = CMD_NOP; /* clear */
}

int main(void)
{
    volatile o10_shm_t *s = shm();
    s->magic = O10_SHM_MAGIC;
    s->arm11_alive = 1;
    s->cmd = CMD_NOP;
    s->status = 0;
    s->wifi_on = 0xFFu;
    s->ftp_on = 0;
    s->ftp_port = 21;
    s->seq = 0;
    s->version = 0x00090101u;

    for (;;) {
        handle_cmd(s);
        s->arm11_alive = 1;
        delay(8000);
    }
    return 0;
}
