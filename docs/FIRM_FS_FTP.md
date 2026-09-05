# File browser + FTP **inside Omni10.firm**

Not the CIA installer — these live in the **ARM9 FIRM** menu.

## Menu

| Entry | Screen |
|-------|--------|
| **FILE BROWSER** | `screen_filebrowser` — list under `sdmc:/` |
| **FTP** | `screen_ftp` — port 5000 toggle / status |

## Status

| Layer | Now | Next |
|-------|-----|------|
| UI navigation | Done | — |
| Real directory listing | Placeholder names | **SDMMC + FAT** driver |
| Open / copy / delete | Message only | After FAT |
| FTP listen | Toggle + status UI | **NWM / sockets** in FIRM |
| FTP LIST/RETR | — | After network stack |

## Why hard on bare metal

- **SD**: ARM9 SDMMC controller + FAT parse (no libctru in FIRM)
- **FTP**: full WiFi (NWM) + TCP — far beyond MCU “NET ON” flag

## Path convention (when SD works)

```text
sdmc:/omni10/
sdmc:/o10/slr/
sdmc:/luma/payloads/Omni10.firm
```
