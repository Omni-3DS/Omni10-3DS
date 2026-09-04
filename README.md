# Omni10-3DS

**Full Access. No Limits.**

Custom Nintendo **3DS FIRM** payload for **Luma3DS** — Old 3DS, New 3DS, New 2DS XL.

Own codebase. Own branding. English UI with DE (and more planned).

---

## Install

1. Build or download **`Omni10.firm`** from [GitHub Actions](https://github.com/Omni-3DS/Omni10-3DS/actions)
2. Copy to: `sd:/luma/payloads/Omni10.firm`
3. Hold **START** while booting → choose **Omni10**

---

## Controls

| Input | Action |
|-------|--------|
| **D-Pad** | Navigate menus |
| **A** | Select / Yes / Confirm |
| **B** | Back / No |
| **START + SELECT** | Power off |

---

## Features

### Working now

| Feature | Notes |
|---------|--------|
| Luma chainload boot | ARM9 entry `0x08000040` |
| RGB888 top (400×240) + bottom (320×240) | Correct landscape orientation |
| Main menu UI | Header, footer, highlight |
| **EN / DE** language | Settings toggle |
| About | Version, branding |
| System info | Old/New 3DS family, entry, FB, lang |
| Internet / WiFi status bar | MCU I2C (LED / wireless flag) — not full NWM stack |
| Internet test screen | Refresh MCU status |
| Settings | Language |
| Confirm dialogs | Reboot / power off |
| **Reboot** / **Power off** | MCU I2C power registers |
| Built-in **.o10** demos | Text runner: PRINT, WAIT, … |
| Left-aligned text | `left_x` + framebuffer mapping |

### o10script (design — on console only)

Scripts are **not** compiled on PC.

```text
sdmc:/o10/slr/name.slr     source on SD (when SDMMC ready)
        │
        │  Omni10 Home → Scripts / o10script
        │  main.c compiles on the 3DS
        ▼
sdmc:/o10/slr/name.o10     opaque bytecode (only Omni10 runs it)
        │
        ▼
   Run on screen
```

| Rule | |
|------|--|
| Compile SLR → O10 | **Only on 3DS** (Omni10 `main.c`) |
| PC CLI / pip | **Does not** produce console `.o10` |
| Related docs repo | [O10-SLR](https://github.com/Omni-3DS/O10-SLR) (docs only, no scripts in git) |

Built-in demo commands (current text runner): `PRINT`, `WAIT`, `CLEAR`, `INFO`, `POWEROFF`, `REBOOT`.

### Planned / in progress

| Feature | Goal |
|---------|------|
| **SDMMC + FAT** | Real `sdmc:/o10/slr/` listing |
| On-device **SLR → opaque O10** | Compile + run from SD |
| File browser | SD / NAND browse |
| FTP server | Over WiFi (needs network stack) |
| Button test / LED test | MCU diagnostics |
| More languages | IT / FR / ES tables |
| Lua runner | Optional later |
| Python port | Optional later (large); same menu/SD layout |
| Splash assets | Custom boot screen |

---

## Menu map (target)

```text
MAIN MENU
├── About
├── System info
├── Scripts / o10script     ← sdmc:/o10/slr/
├── Settings                ← language
├── Internet / WiFi         ← MCU status
├── Button test
├── LED test
├── File browser            ← SD (soon)
├── FTP                     ← soon
├── Reboot
└── Power off
```

---

## Build

Needs **devkitARM** + **firmtool**.

```bash
git clone https://github.com/Omni-3DS/Omni10-3DS.git
cd Omni10-3DS
make firm
# → Omni10.firm
```

CI: `.github/workflows/build.yml` builds on every push.

---

## Docs

| Doc | Topic |
|-----|--------|
| [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) | Design |
| [docs/BOOT.md](docs/BOOT.md) | Boot path |
| [docs/BUILDING.md](docs/BUILDING.md) | Build details |
| [docs/MEMORY_MAP.md](docs/MEMORY_MAP.md) | Memory |
| [docs/o10-scripting.md](docs/o10-scripting.md) | .o10 commands |
| [docs/slr-on-device-flow.md](docs/slr-on-device-flow.md) | SLR on 3DS only |
| [docs/ROADMAP.md](docs/ROADMAP.md) | Roadmap |

---

## Related

- **[O10-SLR](https://github.com/Omni-3DS/O10-SLR)** — o10script docs (no scripts stored in that repo; scripts live on SD)

---

## License

GPL-3.0 — see [LICENSE](LICENSE)

---

**Omni10-3DS** — custom FIRM, own code, maximum control on your console.
