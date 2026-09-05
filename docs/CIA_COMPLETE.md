# O10-Inst-Booter CIA (complete packaging)

## Inputs

| Asset | Path | Role |
|-------|------|------|
| Icon 48×48 | `assets/icons/icon.png` | SMDH |
| Banner 256×128 | `assets/banner/banner.png` | HOME menu banner |
| Sound | `assets/audio/banner.wav` | Banner audio |
| RSF | `installer/meta/app.rsf` | CIA rights |

Generate:

```bash
python3 scripts/gen_assets.py
make -C installer
make -f installer/Makefile.cia cia
```

Needs **bannertool** + **makerom**.

## Release CI

`release.yml` builds firm + 3dsx; CIA step optional when tools present in image.
