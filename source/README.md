# `source/` — modular scaffolding (not the FIRM build yet)

**Important:** The **working Omni10 FIRM** that CI builds is **not** compiled from this folder.

| What | Path | Size |
|------|------|------|
| **Real FIRM payload (built)** | [`firm/arm9/main.c`](../firm/arm9/main.c) | ~29 KB, full UI |
| This folder | `source/*.c` | stubs / future modular split |

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

Planned long-term modular layout (ARM9/ARM11 split, o10 engine modules).  
Files here (`main.c`, `o10.c`, `ui.c`, …) are **scaffolding** — they are **not** linked into Omni10.firm today.
