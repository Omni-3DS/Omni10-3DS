# HOME Scripts (o10 / SLR / Lua)

## Open the hub

| Input | Action |
|-------|--------|
| **X** | Open **HOME SCRIPTS** from main menu |
| Main menu → **HOME SCRIPTS** | Same hub |

> Physical **HOME** is not available on bare-metal HID_PAD (ARM9). **X** is the Scripts/Home shortcut in Omni10.

## Hub entries

| Entry | Behavior |
|-------|----------|
| **.O10** | Run built-in demos (HELLO / SYSCHECK) |
| **.SLR** | On-device path `sdmc:/o10/slr/` + demo compile/run |
| **.LUA** | Placeholder until Lua is embedded (`sdmc:/o10/lua/`) |

## SD paths (when FAT ready)

```text
sdmc:/o10/slr/*.slr → compile on 3DS → *.o10 → run
sdmc:/o10/lua/*.lua → Lua runner (planned)
```
