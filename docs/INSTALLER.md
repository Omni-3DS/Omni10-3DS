# O10-Inst-Booter

Installer / Updater / Booter for Omni10 (citro2d), inspired by
[AuroraOS-Installer](https://github.com/TeamAuroraOS/AuroraOS-Installer).

## Boot sequence

1. App detects `sdmc:/luma/payloads/Omni10.firm` (+ optional `sdmc:/omni10/version.dat`)
2. User taps **Boot Omni10**
3. Prompt: **hold START**
4. On START held → `NS_RebootSystem()`
5. Luma chainloader (START still held) → select **Omni10**

There is no userland API to chainload a Luma payload directly; button-held reboot is the standard approach.

## Auto release

`.github/workflows/release.yml` reads **`version.dat`**, builds FIRM (+ installer if possible), uploads artifacts, creates GitHub Release `vX.Y.Z`.

## Flashcarts

R4 / DSTT / etc. are **DS flashcarts**. Omni10 is a **3DS Luma FIRM**. Not the same path.
