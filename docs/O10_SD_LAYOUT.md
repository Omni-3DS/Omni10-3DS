# SD layout: `o10/`

All Omni10 user data lives under **`sdmc:/o10/`**.

```text
sdmc:/o10/
├── scripts/          # ready-to-run helpers / text notes (optional)
├── slr/              # o10script sources: name.slr  →  compile on 3DS → name.o10
├── lua/              # planned Lua scripts
├── r4/
│   └── backup.bin    # R4 safety backup
├── dstt/
│   └── backup.bin    # DSTT safety backup
└── version.dat       # installed firm version (also written by installer)

sdmc:/luma/payloads/Omni10.firm
```

## `o10/slr/`

| File | Role |
|------|------|
| `*.slr` | Source (readable on PC/SD) |
| `*.o10` | Opaque bytecode — **only Omni10 on the 3DS** compiles/runs |

Example:

```text
sdmc:/o10/slr/hello.slr
```

In Omni10 FIRM: **HOME Scripts** → compile on console → run `.o10`.

## `o10/scripts/`

Optional folder for:

- notes / README for users
- future host-side helpers
- **not** a substitute for on-device SLR→O10 compile

## Installer creates dirs when needed

Backup/restore and install create `o10/`, `o10/r4/`, `o10/dstt/`.  
Users can mkdir `o10/slr` and `o10/scripts` on the SD themselves.
