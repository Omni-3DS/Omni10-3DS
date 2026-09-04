# Omni10 .o10 Scripting (v0.4)

Built-in text scripts executed by the ARM9 payload.

## Commands
| Command | Effect |
|---------|--------|
| `PRINT text` | Draw line on screen |
| `WAIT` | Short delay |
| `CLEAR` | Clear top screen / reset log |
| `INFO` | Print console type (NEW3DS/OLD3DS) |
| `POWEROFF` | Power off |
| `REBOOT` | Reboot |

## Example
```
PRINT HELLO
WAIT
INFO
WAIT
PRINT DONE
```

## Roadmap
- Load `.o10` from SD (needs SDMMC+FAT)
- Binary `.o10` bytecode (from `.slr` compiler)
- More ops: branches, variables, file I/O
