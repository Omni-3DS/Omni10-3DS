# Omni10-3DS Features

## Status bar (header)

| Widget | Source |
|--------|--------|
| **NET ON/OFF/?** | MCU WiFi LED / wireless flag |
| **BAT nn%** (+ if charging) | MCU `0x0B` percent, `0x0F` charge bit |

## Menu

| Entry | Function |
|-------|----------|
| About | Version / branding |
| System info | Console family, entry, FB, lang, WiFi, battery |
| HOME SCRIPTS | .O10 / .SLR / .LUA hub (**X** shortcut) |
| Settings | EN/DE |
| Internet / WiFi | MCU status refresh |
| Battery info | Percent, charging, MCU regs |
| Button test | A/B/X/Y/L/R/Start live |
| LED test | WiFi LED / power LED via MCU |
| File browser | Placeholder (SDMMC next) |
| FTP | Placeholder |
| Reboot / Power off | MCU power |

## Scripts

- **.o10** built-in demos
- **.slr** on-device path `sdmc:/o10/slr/`
- **.lua** reserved `sdmc:/o10/lua/`

## Controls

| Key | Action |
|-----|--------|
| D-Pad | Navigate |
| A / B | Select / Back |
| **X** | HOME Scripts hub |
| START+SELECT | Power off |
