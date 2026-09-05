# O10-Inst-Booter — feature set

## Core
- Install / update Omni10.firm → `sdmc:/luma/payloads/`
- Boot (hold START → Luma chainloader)
- Uninstall
- version.dat display

## Tools
- **File browser** — navigate `sdmc:/`, enter dirs, list files
- **FTP server** — listen port **5000**, minimal USER/PASS/PWD/SYST/QUIT (extend for LIST/RETR)
- Flashcart warning + 5-color LED sequence

## FIRM (Omni10.firm)
Bare-metal payload: UI, scripts, battery, etc. Full SD/FTP in FIRM still need SDMMC + stack; homebrew app covers FS/FTP now.

## Build
```bash
make -C installer
```
Requires citro2d, libctru, network (SOC).
