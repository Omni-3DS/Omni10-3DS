# On-device flow (target)

```
sdmc:/o10/slr/name.slr     readable source on SD
        |
        |  Home -> Scripts -> select name.slr
        |  main.c SLR compiler (on 3DS)
        v
sdmc:/o10/slr/name.o10     opaque binary (not human-readable)
        |
        |  Run O10
        v
   Omni10 bytecode interpreter (only Omni10 runs it)
```

## Roles

| Piece | Job |
|-------|-----|
| **Python O10-SLR + Actions** | Format, tests, PC compile for developers |
| **main.c on 3DS** | Compile SLR→O10 on console + run O10 + UI |
| **Path** | `sdmc:/o10/slr/` |

## Status

- Format + PC tools: **O10-SLR** repo (Python + C compiler)
- On-device compiler + runner: designed for main.c (v0.5.0)
- SDMMC read of `sdmc:/o10/slr/`: next (needs FAT)
- Until SD works: built-in hello/demo/sys SLR sources compile on-device the same way
