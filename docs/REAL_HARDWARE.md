# Omni10 — what is REAL hardware (v1.0.0+)

## Real (this tree)

| Component | Implementation |
|-----------|----------------|
| **SDMMC/TMIO** | `firm/arm9/sdmmc.c` — registers `0x10006000`, CMD0/8/55/ACMD41, CID/RCA/select, **sector read/write** |
| **Card detect** | `EMMC_STATUS0` bit `SIGSTATE` |
| **MBR read** | `fat_min.c` → sector 0 via real read |
| **MCU battery / LED** | I2C MCU regs (existing ARM9) |
| **ARM11 core** | Second FIRM section + SHM mailbox |

## Not magic / still limited

| Component | Why |
|-----------|-----|
| Full FAT32 path names `o10/r4/backup.bin` | Needs full FAT cluster chain; sector R/W is the foundation |
| Nintendo WiFi NWM + TCP FTP | Not present in bare FIRM; needs NWM firmware/services |
| R4/DSTT cart dump bus | Separate SPI/cart protocol on top of SD backup targets |

## Goal

No more “BACKUP OK” without `sdmmc_writesectors`. UI that claims SD must call `sdmmc_init` / `sdmmc_readsectors`.
