# Omni10-3DS

**Full Access. No Limits.**

Custom Nintendo 3DS FIRM payload — boots from Luma3DS chainloader on Old 3DS and New 3DS / New 2DS XL.

## Status (v0.2.0)

| Feature | State |
|---------|--------|
| Boot under Luma chainload | Working |
| RGB888 framebuffer draw | Working |
| 8x8 text + main menu | Working |
| Old 3DS + New 2DS XL | Working |
| START+SELECT power off | Working |
| Menu reboot | Working |
| System info (O3DS/N3DS detect) | Working |
| File browser / .o10 / FTP | Planned |

## Install

1. Download `Omni10.firm` from [Actions artifacts](https://github.com/Omni-3DS/Omni10-3DS/actions) or Releases  
2. Copy to `sd:/luma/payloads/Omni10.firm`  
3. Hold **START** on boot → select **Omni10**

## Controls

| Input | Action |
|-------|--------|
| D-Pad | Navigate menu |
| **A** | Select |
| **B** | Back |
| **START + SELECT** | Power off |

## Build

```bash
# needs devkitARM + firmtool
make firm
```

GitHub Actions builds on every push (`devkitpro/devkitarm` container + firmtool `-i`).

```
firmtool build Omni10.firm -i -n 0x08000040 -e 0 -D arm9.bin -A 0x08000040 -C NDMA
```

## Roadmap

1. SD file browser (FAT)
2. Native `.o10` scripts
3. Lua API
4. Real FTP server
5. Optional ARM11 section

## License

GPL-3.0
