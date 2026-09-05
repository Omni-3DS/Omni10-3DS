# O10-Inst-Booter

**Omni10 Installer · Updater · Booter** (citro2d UI).

Boot flow modeled after [AuroraOS-Installer](https://github.com/TeamAuroraOS/AuroraOS-Installer):
Luma only opens the payload menu if **START** is held at boot — so Boot asks you to hold START, then reboots.

## Flow

```
Installed & ready
  → Boot Omni10
  → Hold START
  → Reboot
  → Luma3DS chainloader
  → Select Omni10
```

## SD paths

| File | Path |
|------|------|
| Firm | `sdmc:/luma/payloads/Omni10.firm` |
| Version | `sdmc:/omni10/version.dat` |

Install copies from `sdmc:/Omni10.firm` or `sdmc:/omni10/Omni10.firm`.

## Build

```bash
make -C installer
# O10-Inst-Booter.3dsx
```

CIA: optional `bannertool` + `makerom` + `meta/app.rsf`.

## Flashcarts (R4 / DSTT / …)

Omni10 is a **Luma FIRM payload**, not a DS flashcart kernel.
R4/DSTT-style carts are not how you run Omni10.firm — use Luma3DS CFW on the 3DS.
