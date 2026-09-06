# Omni10-3DS

Custom **Nintendo 3DS FIRM** (ARM9 + ARM11). Not GodMode9 — own codebase.

## Status (v1.1.0)

| Layer | Real? |
|-------|-------|
| Dual-core FIRM (ARM9 NDMA + ARM11 XDMA) | Yes |
| TMIO SDMMC sector R/W (`0x10006000`) | Yes |
| FAT32 mount + root list + file read | Yes |
| O10BK1 backup region (tail of volume) | Yes |
| MCU battery / notification LED | Yes |
| R4/DSTT UI flow + real SD backup/restore | Yes |
| `.o10` / mini Lua runners | Yes |
| Full NWM WiFi + TCP FTP sockets | Not in bare FIRM |

## Build

```bash
# needs devkitARM + firmtool
make
# -> Omni10.firm
```

Boot via Luma chainloader.

## SD layout

- `o10/scripts`, `o10/slr`, `o10/lua` (paths for future FAT create)
- Hardware backup: last **2048 sectors** of the FAT volume, magic `O10BK1`

## Docs

- `docs/REAL_HARDWARE.md`
- `docs/ARM11.md`
- `docs/FLASHCART_WARN.md`

## License

GPL-3.0 (see repo).
