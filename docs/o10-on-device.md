# On-device SLR → O10 (Omni10-3DS)

## Flow

1. Scripts live at **`sdmc:/o10/slr/name.slr`** (readable source on SD).
2. Home → **Scripts** → select `name.slr`.
3. **A** = **compile on console** → opaque binary **`.o10`** in RAM (not human-readable).
4. **A** again = **run `.o10`** (only Omni10 understands the bytecode).

## Who does what

| Component | Role |
|-----------|------|
| **Python / O10-SLR CI** | Defines format, tests, PC tools |
| **main.c (3DS)** | Compiles SLR → O10 and executes O10 |
| **SD path** | `sdmc:/o10/slr/*.slr` |

Until SDMMC is wired, built-in `hello.slr` / `demo.slr` / `sys.slr` are compiled on-device the same way.

## Opaque .o10

Header: `O10\0` + version + string table. Opcodes only Omni10 runs.
