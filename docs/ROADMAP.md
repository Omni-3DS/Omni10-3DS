# Omni10-3DS Roadmap

## Done

- [x] FIRM boot under Luma (Old + New)
- [x] Framebuffer UI (top + bottom)
- [x] Main menu + navigation
- [x] EN/DE language
- [x] System info
- [x] MCU power off / reboot
- [x] WiFi status via MCU
- [x] Built-in text .o10 demos
- [x] o10 binary format + SLR compile scaffolding (`source/o10.c`)
- [x] Lua API surface docs + stub (`lua_omni`)
- [x] GitHub Actions CI

## Next (o10script)

- [ ] Wire `o10_slr_compile` + `o10_run_prog` into `firm/arm9/main.c` UI
- [ ] SDMMC + FAT
- [ ] List / load `sdmc:/o10/slr/*.slr`
- [ ] Write opaque `.o10` to SD after compile
- [ ] Opcodes v2: JMP, KEY, SET

## Later (Lua)

- [ ] Embed minimal Lua VM
- [ ] `ui` / `sys` bindings
- [ ] `o10` bridge from Lua
- [ ] `fs` after FAT
- [ ] `net` after stack

## Later (platform)

- [ ] File browser
- [ ] FTP
- [ ] More languages
