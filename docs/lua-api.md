# Omni10 Lua API

Lua is a **planned** optional runner alongside **o10script**.  
Same SD layout idea; Lua does **not** replace on-device SLR→O10.

## Status

| Piece | Status |
|-------|--------|
| o10script / .o10 | Primary, in FIRM (expanding) |
| Lua 5.x embed | Planned (large) |
| This API doc | Target surface for implementers |

## Paths (planned)

```text
sdmc:/o10/lua/*.lua
sdmc:/o10/luapackages/
```

Menu: **Scripts** → o10script **or** Lua (when embedded).

---

## Modules (target)

### `ui`
`echo`, `ask`, `ask_input`, `ask_selection`, `notify`, `progress`, `check_key`

### `fs`
`exists`, `is_dir`, `list`, `read`, `write`, `copy`, `move`, `delete`, `mkdir`

### `sys`
`info`, `version`, `reboot`, `poweroff`, `brightness`, `led`

### `net`
`is_connected`, `ftp_start`, `ftp_stop` (needs stack)

### `o10` (bridge)
| Function | Meaning |
|----------|---------|
| `o10.run_file(path)` | Run a `.o10` blob |
| `o10.compile_slr(src)` | Only if host allows (3DS) |

---

## Example (target)

```lua
local i = sys.info()
ui.echo("Model: " .. tostring(i.model))
o10.run_file("sdmc:/o10/slr/hello.o10")
```

## Implementation order

1. Solid o10 binary runner + SD load  
2. Minimal Lua VM embed (subset)  
3. `ui` + `sys` bindings  
4. `fs` after FAT  
5. `net` last  

*API grows as modules are implemented in FIRM.*
