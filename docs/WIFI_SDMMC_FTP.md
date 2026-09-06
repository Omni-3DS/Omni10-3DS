# WiFi / SDMMC / FTP in Omni10 FIRM

## Dual-core (v0.9.1+)

ARM11 section is packed into `Omni10.firm`. Shared memory at `0x1FF80E00` holds WiFi/FTP flags.

## WiFi

| Layer | Status |
|-------|--------|
| MCU wireless flag (ARM9 I2C) | **Live** |
| ARM11 SHM wifi_on | **Live IPC** |
| SSID list / TCP | needs NWM/soc (not in bare FIRM) |

## SDMMC

| Layer | Status |
|-------|--------|
| Path layout UI | **Yes** |
| Soft probe | **Yes** |
| Full FAT | expanding |

## FTP

| Layer | Status |
|-------|--------|
| FTP UI + ARM11 ftp_on | **Yes** |
| Real listen :21 | needs TCP stack |
