# Omni10-3DS – 3DS Memory Map

Public layout based on community documentation (3dbrew / GBATEK).  
Used for our **own** from-scratch FIRM design.

## Physical memory (overview)

### Shared / both cores

| Address      | Size     | Description        |
|--------------|----------|--------------------|
| 0x10000000   | ~128 MB  | IO registers       |
| 0x18000000   | 6 MB     | VRAM (A + B)       |
| 0x1FF00000   | 512 KB   | DSP / shared       |
| 0x1FF80000   | 512 KB   | AXI WRAM           |
| 0x20000000   | 128 MB   | FCRAM (O3DS)       |
| 0x28000000   | +128 MB  | FCRAM ext (N3DS)   |

### ARM9-specific

| Address      | Size    | Description              |
|--------------|---------|--------------------------|
| 0x08000000   | 1 MB    | ARM9 internal RAM        |
| 0x08100000   | 512 KB  | N3DS ARM9 extension      |
| 0xFFF00000   | 16 KB   | DTCM (bootrom mapping)   |
| ITCM         | 32 KB   | Instruction TCM          |

### ARM11-specific

| Address      | Size   | Description                |
|--------------|--------|----------------------------|
| 0x17E00000   | 8 KB   | MPCore private             |
| 0x17E10000   | 4 KB   | L2C (N3DS)                 |
| 0x1F000000   | 4 MB   | N3DS extra                 |

## Screen sizes (for splash / UI)

| Screen | Resolution |
|--------|------------|
| Top    | 400 × 240  |
| Bottom | 320 × 240  |

Framebuffer layout depends on GPU config in the payload. Omni10 will define its own framebuffer base once screen init is implemented.

## Omni10 planned load regions (skeleton)

These are **working targets** for our linker scripts and will be refined when the real FIRM packing is wired:

| Section   | Role                          |
|-----------|-------------------------------|
| ARM9      | Code in ARM9 RAM / FCRAM slice|
| ARM11     | Code in AXI WRAM / FCRAM      |
| Data      | Splash, fonts, config embed   |

Exact ORIGIN/LENGTH values live in `linker/arm9.ld` and `linker/arm11.ld` and in `include/hw/memory_map.h`.

## References (public)

- https://www.3dbrew.org/wiki/Memory_layout
- GBATEK 3DS Memory and I/O Map

## Policy

Omni10 does **not** copy GodMode9 source. Hardware addresses are public knowledge for homebrew.
