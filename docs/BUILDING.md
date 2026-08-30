# Building Omni10-3DS

## Why you saw "invalid firm or corrupted firm"

Older CI builds wrote a **text placeholder** named `Omni10.firm`.  
That is **not** a FIRM binary. boot9strap / Luma correctly reject it.

**Policy now:** the build system **never** writes a fake `.firm`.

## Requirements for a real FIRM

1. [devkitPro / devkitARM](https://devkitpro.org/)
2. `pip install firmtool pycryptodome`
3. Working ARM9 + ARM11 binaries (still in progress in this repo)
4. `firmtool` to pack sections into a proper FIRM

```bash
export DEVKITPRO=/opt/devkitpro   # or your path
export DEVKITARM=$DEVKITPRO/devkitARM
make check
make firm   # only succeeds when real binaries can be produced
```

## Current status

| Step                         | Status        |
|-----------------------------|---------------|
| Refuse fake .firm           | Done          |
| Source layout ARM9/ARM11    | Done          |
| Memory map / linker scripts | Done          |
| Compile + link real bins    | Not yet       |
| firmtool pack               | Not yet       |
| Bootable on hardware        | Not yet       |

Until compile/link works, **do not** put any `Omni10.firm` from this project on your SD card for booting.
