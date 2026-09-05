# Omni10 assets

**PNGs for Universal-Updater, CIA banner, splash.**

If this folder looks empty on a fresh clone before CI ran:

```bash
python3 scripts/gen_assets.py
```

Then commit `assets/` or run Actions → **Commit assets PNGs**.

## Layout

| File | Use |
|------|-----|
| `icon.png` | Universal-Updater / SMDH 48×48 |
| `icons/icon.png` | same |
| `icons/icon-256.png` | larger preview |
| `banner/banner.png` | CIA / HOME banner 256×128 |
| `splash/splash_top.png` | FIRM splash 400×240 |
| `audio/banner.wav` | CIA banner sound |

## Universal-Updater

Point your UU entry `icon` URL at:

`https://raw.githubusercontent.com/Omni-3DS/Omni10-3DS/main/assets/icon.png`
