# Omni10 Lua API Documentation

Omni10 includes a full **Lua 5.4** implementation with an extended API designed for power and clarity.

## Running Lua Scripts

There are several ways to run Lua scripts:

1. Select a `.lua` file in the file browser → **Execute Lua script**
2. Place scripts in `0:/omni10/luascripts/` → HOME → Lua Scripts
3. Place scripts in `data/luascripts/` (bundled at build time)
4. `data/autorun.lua` (runs automatically on boot if built with `SCRIPT_RUNNER=1`)

## Package System

```lua
package.path = "0:/omni10/luapackages/?.lua;0:/omni10/luapackages/?/init.lua;V:/luapackages/?.lua;V:/luapackages/?/init.lua"
```

Use `require("mymodule")` as usual.

---

## Core Modules

### `ui` – User Interface

| Function | Description |
|----------|-------------|
| `ui.echo(msg)` | Show a message and wait for confirmation |
| `ui.ask(msg)` | Yes/No dialog. Returns `true` / `false` |
| `ui.ask_input(prompt, default)` | Text input dialog |
| `ui.ask_selection(title, options)` | Selection list. Returns index or `nil` |
| `ui.show_qr(data)` | Display QR code |
| `ui.check_key(key)` | Check if a key is currently pressed |
| `ui.notify(msg)` | Non-blocking notification |
| `ui.progress(title, current, total)` | Progress bar |

### `fs` – File System

| Function | Description |
|----------|-------------|
| `fs.exists(path)` | Check if path exists |
| `fs.is_dir(path)` | Check if path is a directory |
| `fs.list(path)` | List directory contents |
| `fs.find_all(path, pattern)` | Recursive search |
| `fs.copy(src, dst)` | Copy file/directory |
| `fs.move(src, dst)` | Move / rename |
| `fs.delete(path)` | Delete file or directory |
| `fs.mkdir(path)` | Create directory |
| `fs.read(path)` | Read entire file as string |
| `fs.write(path, data)` | Write string to file |
| `fs.hash(path, algo)` | Calculate hash (`"sha256"`, `"sha1"`, `"md5"`) |
| `fs.ask_select_file(title, path)` | File picker |
| `fs.ask_select_dir(title, path)` | Directory picker |

### `sys` – System Information & Control

| Function | Description |
|----------|-------------|
| `sys.info()` | Returns table with console info (model, region, serial, etc.) |
| `sys.version()` | Omni10 version string |
| `sys.reboot()` | Reboot the console |
| `sys.poweroff()` | Power off |
| `sys.brightness(level)` | Set screen brightness (0-5) |
| `sys.led(color, duration)` | Control notification LED |

### `net` – Networking (Omni10 exclusive)

| Function | Description |
|----------|-------------|
| `net.ftp_start(port, user, pass)` | Start FTP server |
| `net.ftp_stop()` | Stop FTP server |
| `net.ftp_status()` | Returns status table |
| `net.ftp_client(host, port, user, pass)` | Simple FTP client helpers |
| `net.is_connected()` | Check network status |

### `nand` – NAND Operations

| Function | Description |
|----------|-------------|
| `nand.mount(partition)` | Mount a NAND partition |
| `nand.unmount(partition)` | Unmount |
| `nand.backup(path)` | Create NAND backup |
| `nand.restore(path)` | Restore from backup (with safety checks) |

### `title` – Title / CIA Handling

| Function | Description |
|----------|-------------|
| `title.list()` | List installed titles |
| `title.info(id)` | Get title information |
| `title.install(cia_path)` | Install CIA |
| `title.build_cia(path)` | Build CIA from folder / content |

---

## Example

```lua
local info = sys.info()
ui.echo("Console: " .. info.model .. "\nRegion: " .. info.region)

if ui.ask("Start FTP server on port 5000?") then
    net.ftp_start(5000, "omni", "omni10")
    ui.echo("FTP running. Press A to stop.")
    net.ftp_stop()
end
```

---

*This document will grow as the API is implemented.*
