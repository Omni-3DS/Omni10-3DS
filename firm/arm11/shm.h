#pragma once
/* Shared with ARM9 (same layout / address). */
#define O10_SHM_MAGIC  0x4F313041u
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
    unsigned int magic;
    unsigned int arm11_alive;
    unsigned int cmd;
    unsigned int arg0;
    unsigned int arg1;
    unsigned int status;
    unsigned int wifi_on;
    unsigned int ftp_on;
    unsigned int ftp_port;
    unsigned int seq;
    unsigned int version;
    unsigned int reserved[5];
} o10_shm_t;
