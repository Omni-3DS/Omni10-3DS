# ARM11 section (Omni10 FIRM)

Goal: dual-processor FIRM — **ARM9** = FS/scripts/power, **ARM11** = screen/GPU assist.

| Item | Status |
|------|--------|
| Entry stub + link at `0x1FF80000` | Scaffold |
| firmtool multi-section package | Next |
| Shared ring buffer with ARM9 | Planned |
| Framebuffer fill from ARM11 | Planned |

Build (standalone ELF for now):

```bash
arm-none-eabi-gcc -marm -march=armv6k -mtune=mpcore -nostdlib -T link.ld start.s main_arm11.c -o arm11.elf
```

Full FIRM will pass both ARM9.bin and ARM11.bin to `firmtool`.
