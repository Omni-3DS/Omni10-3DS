# Omni10 ARM11 core

Loaded at **0x1FF80000** (AXI WRAM) via firmtool **XDMA**.

## Shared memory (`0x1FF80E00`)

| Field | Meaning |
|-------|---------|
| magic | `O10A` |
| arm11_alive | 1 while loop runs |
| cmd / arg0 / arg1 | ARM9 → ARM11 |
| status | ARM11 response |
| wifi_on / ftp_on | state |

Commands: PING, WIFI_STATUS, FTP_START/STOP/STATUS, SET_LED, GET_VERSION.

## Build

Top-level `Makefile` builds `arm11.bin` and packs it into `Omni10.firm` with ARM9.

## Limits

Bare FIRM does **not** get Nintendo `soc:u` / NWM. ARM11 owns dual-core IPC and FTP/WiFi **state**; full TCP needs further kernel/homebrew work.
