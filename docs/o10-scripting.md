# Omni10 `.o10` Scripting Language

`.o10` is the native high-level scripting language of Omni10-3DS.

It is designed to be simpler and more readable than classic GodMode9 `.gm9` scripts while still being powerful enough for complex automation.

## File Extension

- **`.o10`** → Native Omni10 scripts

## Basic Syntax

```o10
# This is a comment

echo "Hello from Omni10"

set NAME "Omni10-3DS"
echo "Running on $[NAME]"

if exists "0:/omni10/config.txt"
    echo "Config found"
else
    echo "No config found"
end
```

## Variables

- Variables use the form `$[VARNAME]`
- Created/updated with `set`

```o10
set BACKUP_PATH "0:/omni10/backups"
echo $[BACKUP_PATH]
```

## Core Commands

| Command | Description |
|---------|-------------|
| `echo <msg>` | Print message |
| `ask <msg>` | Yes/No prompt |
| `input <var> <prompt>` | Text input |
| `set <var> <value>` | Set variable |
| `if <condition>` / `else` / `end` | Conditional |
| `for <path> <pattern>` / `next` | Loop over files |
| `goto <label>` | Jump to label |
| `label <name>` | Define label |
| `run <script>` | Run another script |
| `copy <src> <dst>` | Copy |
| `move <src> <dst>` | Move |
| `delete <path>` | Delete |
| `mkdir <path>` | Create directory |
| `hash <path> <algo>` | Calculate hash |
| `ftp start [port]` | Start FTP server |
| `ftp stop` | Stop FTP server |
| `reboot` | Reboot console |
| `poweroff` | Power off |

## Conditions

```o10
if exists "0:/file.txt"
if isdir "0:/folder"
if equal $[VAR] "value"
if key A
```

## Example Script

```o10
# Omni10 Example – Create backup folder and start FTP

echo "=== Omni10 Backup Helper ==="

set OUT "0:/omni10/out"
mkdir $[OUT]

if ask "Start FTP server now?"
    ftp start 5000
    echo "FTP running on port 5000"
    echo "User: omni  /  Pass: omni10"
    ask "Press A when finished"
    ftp stop
end

echo "Done."
```

## Compatibility

Legacy GodMode9 `.gm9` scripts will be supported in compatibility mode, but new development should use `.o10` or Lua.

---

*Documentation will be expanded as the parser is implemented.*
