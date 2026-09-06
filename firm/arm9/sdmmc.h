#pragma once
/* Omni10 — real TMIO SD/MMC controller @ 0x10006000 (3dbrew/GBATEK). */

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;

#define SDMMC_BASE 0x10006000u

#define REG_SDCMD      0x00
#define REG_SDPORTSEL  0x02
#define REG_SDCMDARG0  0x04
#define REG_SDCMDARG1  0x06
#define REG_SDSTOP     0x08
#define REG_SDBLKCOUNT 0x0A
#define REG_SDRESP0    0x0C
#define REG_SDSTATUS0  0x1C
#define REG_SDSTATUS1  0x1E
#define REG_SDIRMASK0  0x20
#define REG_SDIRMASK1  0x22
#define REG_SDCLKCTL   0x24
#define REG_SDBLKLEN   0x26
#define REG_SDOPT      0x28
#define REG_SDFIFO     0x30
#define REG_SDRESET    0xE0

#define TMIO_STAT0_CMDRESPEND 0x0001
#define TMIO_STAT0_DATAEND    0x0004
#define TMIO_STAT0_CARD_REMOVE 0x0008
#define TMIO_STAT0_CARD_INSERT 0x0010
#define TMIO_STAT0_SIGSTATE   0x0020

#define TMIO_STAT1_CMD_IDX_ERR 0x0001
#define TMIO_STAT1_CRCFAIL     0x0002
#define TMIO_STAT1_STOPBIT_ERR 0x0004
#define TMIO_STAT1_DATATIMEOUT 0x0008
#define TMIO_STAT1_RXOVERFLOW  0x0010
#define TMIO_STAT1_TXUNDERRUN  0x0020
#define TMIO_STAT1_CMDTIMEOUT  0x0040
#define TMIO_STAT1_RXRDY       0x0100
#define TMIO_STAT1_TXRQ        0x0200

#define TMIO_MASK_ALL 0x837F031Du
#define TMIO_MASK_GW  (TMIO_STAT1_CMD_IDX_ERR|TMIO_STAT1_CRCFAIL|TMIO_STAT1_STOPBIT_ERR|\
                       TMIO_STAT1_DATATIMEOUT|TMIO_STAT1_RXOVERFLOW|TMIO_STAT1_TXUNDERRUN|\
                       TMIO_STAT1_CMDTIMEOUT)

/* SD commands */
#define CMD_GO_IDLE     0x0400
#define CMD_SEND_IF_COND 0x0408
#define CMD_ALL_SEND_CID 0x0202
#define CMD_SEND_REL_ADDR 0x0403
#define CMD_SELECT_CARD 0x0417
#define CMD_SEND_CSD    0x0409
#define CMD_SET_BLOCKLEN 0x0410
#define CMD_READ_SINGLE 0x0431
#define CMD_WRITE_SINGLE 0x0618
#define CMD_APP_CMD     0x0437
#define ACMD_SD_SEND_OP_COND 0x0469

int  sdmmc_init(void);
int  sdmmc_card_inserted(void);
int  sdmmc_is_sdhc(void);
u32  sdmmc_get_sectors(void);
int  sdmmc_readsectors(u32 sector, u32 count, u8 *out);
int  sdmmc_writesectors(u32 sector, u32 count, const u8 *in);
