# `source/` — modular scaffolding (not the FIRM build yet)

**Important:** The **working Omni10 FIRM** that CI builds is **not** compiled from this folder.

| What | Path | Size |
|------|------|------|
| **Real FIRM payload (built)** | [`firm/arm9/main.c`](../firm/arm9/main.c) | ~29 KB, full UI |
| This folder | `source/*.c` | modular development code |

## What is in the real FIRM (`firm/arm9/main.c`)

- MAIN MENU (12 entries)
- **HOME SCRIPTS** (o10 / SLR / Lua hub)
- **X button** → opens HOME SCRIPTS
- System info, Internet/WiFi, Battery %
- Button test, LED test
- File browser UI, FTP UI
- Settings EN/DE, Reboot, Power off (START+SELECT)

Open and search:

```
https://github.com/Omni-3DS/Omni10-3DS/blob/main/firm/arm9/main.c
```

Strings: `HOME SCRIPTS`, `screen_scripts_hub`, `screen_filebrowser`, `BTN_X`

## Build

```bash
make firm          # uses firm/arm9/main.c (decode from scripts/firm_main_*.zlib.b64 if needed)
```

## This `source/` tree

Planned long-term modular layout (ARM9/ARM11 split, o10 engine modules). Files
here are **not** linked into `Omni10.firm` today. `fs.c` provides a POSIX
host backend for the filesystem API, useful to tools and host-side development;
it is not a bare-metal SDMMC/FAT driver. Set `OMNI_SD_ROOT` to choose the SD
card directory (defaults to `./sdmc`). See
[`docs/FILESYSTEM_API.md`](../docs/FILESYSTEM_API.md) for path rules and
operation behavior.
