# Omni10-3DS

**Full Access. No Limits.** Custom 3DS FIRM + installer — built to go beyond classic payloads (see [docs/VS_GM9.md](docs/VS_GM9.md)).

## Components

| Piece | Role |
|-------|------|
| **Omni10.firm** | Luma payload (ARM9, ARM11 scaffold) |
| **O10-Inst-Booter** | CIA/3dsx installer · updater · booter + banner **sound** |
| **o10script** | On-console `.slr` → opaque `.o10` |
| **Lua** | Planned second runner |
| **Assets** | `scripts/gen_assets.py` → icon / banner / splash / wav |

## Install firm

1. Release artifact `Omni10.firm` → `sdmc:/luma/payloads/Omni10.firm`
2. Or use **O10-Inst-Booter** → Install → **Boot** (hold **START** → Luma → Omni10)

## Build

```bash
make firm
python3 scripts/gen_assets.py
make -C installer          # 3dsx
# make -f installer/Makefile.cia cia   # needs bannertool + makerom
```

CI: `version.dat` → automatic GitHub Release.

## Docs

- [FEATURES](docs/FEATURES.md) · [FIRM FS/FTP](docs/FIRM_FS_FTP.md) · [CIA](docs/CIA_COMPLETE.md)
- [o10 / Lua / assets](docs/O10_LUA_ASSETS.md) · [ARM11](firm/arm11/README.md)

## License

GPL-3.0
