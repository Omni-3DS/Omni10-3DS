# Omni10-3DS – Built From Scratch

**No GodMode9 fork.** This is an independent project.

## Why no fork?

We want our own architecture, own branding, own scripting (`.o10`), own FTP design, and full control over the codebase.

Inspiration from the 3DS homebrew scene is fine. Copying GM9 source is not the goal.

## Architecture (own design)

```
Omni10.firm
├── ARM9 section     → FS, crypto, NAND, low-level, host for scripts
├── ARM11 section    → UI, splash, input, FTP/network
└── Embedded data    → splash, fonts, config, autorun
```

## Road to a real .firm

1. **Toolchain** – devkitARM + firmtool locally or in CI
2. **Boot stubs** – `source/arm9/start.s`, `source/arm11/start.s` (started)
3. **Memory map** – correct 3DS physical addresses in linker scripts
4. **Minimal screen** – clear framebuffer + draw text (splash)
5. **SD access** – init SDMMC + simple read
6. **Then** – UI, `.o10`, Lua, FTP

## Current status

| Piece              | Status        |
|--------------------|---------------|
| Project + CI       | Done          |
| Languages EN–ES    | Done          |
| Module API stubs   | Done          |
| ARM9/ARM11 stubs   | Started       |
| Linker scripts     | Skeleton      |
| Real hardware init | Not yet       |
| Bootable .firm     | Not yet (0 KB placeholder only) |

## Honest note

A full bootable FIRM is a large project. Progress is step by step on original code only.
