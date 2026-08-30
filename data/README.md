# Data / Bundled Assets

Files in this folder can be embedded into the final `.firm` at build time.

## Current contents

| File                    | Description                          |
|-------------------------|--------------------------------------|
| `config.default.ini`    | Default configuration                |
| `autorun.o10.example`   | Example autorun script               |

## Planned / to add

- `splash.png` (or converted binary) – official splash
- `autorun.o10` / `autorun.lua` – real autorun
- Fonts (FRF / bitmap)
- Default themes
- Built-in scripts

## Usage

The Makefile will later convert and embed these assets automatically into the FIRM (similar to GodMode9 VRAM0).
