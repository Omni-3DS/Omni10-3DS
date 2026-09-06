# WiFi / SDMMC / FTP in Omni10 FIRM

## WiFi

| Layer | Status |
|-------|--------|
| MCU wireless flag / LED | **Live** (regs `0x61`, `0x2A`) |
| SSID list / scan | Needs wireless firmware + ARM11 |
| TCP/IP (`soc:u`) | **ARM11 homebrew path** |

FIRM shows status bar + INTERNET screen. Scripts: `WIFI` / `wifi()`.

## SDMMC

| Layer | Status |
|-------|--------|
| SD present probe UI | **Yes** |
| Path layout `o10/scripts`, `o10/slr`, `o10/r4`, `o10/dstt` | **Documented** |
| Full FAT read/write in ARM9 FIRM | Partial — expanding |

## FTP

| Layer | Status |
|-------|--------|
| FTP UI (port 21, user) | **Yes** |
| Real socket listen | Needs TCP stack (ARM11) |

When WiFi TCP is available on dual-core builds, FTP binds `0.0.0.0:21`.
