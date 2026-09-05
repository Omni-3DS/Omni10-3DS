# o10 · Lua · assets

## o10script
- Compile **only on 3DS** → opaque `.o10`
- Path: `sdmc:/o10/slr/`
- Engine: `source/o10.c` + FIRM runner

## Lua
- Optional runner `sdmc:/o10/lua/`
- Stub: `source/lua_omni.c`
- API surface: `docs/lua-api.md`

## Assets
```bash
python3 scripts/gen_assets.py
```
Produces icon, banner, splash, CIA banner WAV under `assets/`.
