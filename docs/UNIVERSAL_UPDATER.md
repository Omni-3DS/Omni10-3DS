# Universal-Updater / app store entry — Omni10

Fill the UU / app form roughly like this.

## Identity

| Field | Value |
|-------|--------|
| **Title** | Omni10 / O10-Inst-Booter |
| **Author** | Omni-3DS |
| **Category** | utility / payload / CFW tool |
| **Version** | from `version.dat` (e.g. `0.8.1`) |

## Description (short)

```text
Omni10-3DS installer, updater and booter. Installs Omni10.firm for Luma3DS,
boots via START-hold into the Luma chainloader, optional R4/DSTT safety backup.
```

## Description (long)

```text
O10-Inst-Booter (citro2d):
• Install / update / uninstall Omni10.firm → sdmc:/luma/payloads/
• Boot Omni10: hold START → Luma payload menu → select Omni10
• R4/DSTT: auto backup to sdmc:/o10/r4/backup.bin or o10/dstt/backup.bin + restore
• Scripts path: sdmc:/o10/slr/ (o10script on console), sdmc:/o10/scripts/

Omni10.firm is a custom Luma payload (not GodMode9).
Requires Luma3DS CFW.
```

## Downloads (release assets — normal names)

| File | URL pattern |
|------|-------------|
| CIA | `…/releases/latest/download/O10-Inst-Booter.cia` |
| 3DSX | `…/releases/latest/download/O10-Inst-Booter.3dsx` |
| FIRM | `…/releases/latest/download/Omni10.firm` |

Repo: `https://github.com/Omni-3DS/Omni10-3DS`

Latest release:
`https://github.com/Omni-3DS/Omni10-3DS/releases/latest`

## Icon

```text
https://raw.githubusercontent.com/Omni-3DS/Omni10-3DS/main/assets/icon.png
```

(Run Actions → **Commit assets PNGs** if `assets/icon.png` is missing.)

## Screenshots

Optional: add `assets/screenshots/` later and link raw GitHub URLs.

## JSON snippet (typical UU entry style)

```json
{
  "title": "O10-Inst-Booter",
  "author": "Omni-3DS",
  "description": "Omni10 installer, updater and Luma booter. Paths: o10/slr, o10/scripts.",
  "version": "0.8.1",
  "category": ["utility"],
  "console": ["3DS"],
  "icon_url": "https://raw.githubusercontent.com/Omni-3DS/Omni10-3DS/main/assets/icon.png",
  "download_url_3dsx": "https://github.com/Omni-3DS/Omni10-3DS/releases/latest/download/O10-Inst-Booter.3dsx",
  "download_url_cia": "https://github.com/Omni-3DS/Omni10-3DS/releases/latest/download/O10-Inst-Booter.cia"
}
```

Adjust version when you bump `version.dat`.
