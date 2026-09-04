# Omni10 o10script / .o10

**Compile `.slr` → `.o10` only on the 3DS** (Omni10 `main.c`).  
PC tools do not produce runnable console `.o10`.

## Paths

| Path | Role |
|------|------|
| `sdmc:/o10/slr/*.slr` | Source (readable) |
| `sdmc:/o10/slr/*.o10` | Opaque bytecode (not human-readable) |
| Built-in demos | Until SDMMC is ready |

## Flow

```
Home → Scripts / o10script
  → pick name.slr from sdmc:/o10/slr/
  → on-device compile → name.o10
  → run bytecode
```

---

## Text mode (current FIRM demos)

Line-oriented, uppercase commands:

| Command | Args | Effect |
|---------|------|--------|
| `PRINT` | text… | Draw a log line |
| `WAIT` | [ms] | Delay (default short) |
| `CLEAR` | — | Clear top / reset log |
| `INFO` | — | Show NEW3DS / OLD3DS |
| `POWEROFF` | — | Power off |
| `REBOOT` | — | Reboot |
| `HALT` | — | End script |

```
PRINT HELLO FROM O10
WAIT
INFO
PRINT DONE
```

---

## Binary .o10 (opaque)

### Header (8 bytes)

```
0..3  magic     'O' '1' '0' 0
4     version   u8  (1)
5     flags     u8
6..7  strtab    u16 LE  absolute offset of string table
```

Then: **code** bytes, then **string table** (NUL-terminated strings).

### Opcodes (v1)

| Op | Code | Operands | Meaning |
|----|------|----------|---------|
| HALT | 0x00 | — | Stop |
| PRINT | 0x01 | u16 str_off | Print string from strtab |
| WAIT | 0x02 | u16 ms | Delay |
| CLEAR | 0x03 | — | Clear screen log |
| INFO | 0x04 | — | Console family |
| POWEROFF | 0x05 | — | Power off |
| REBOOT | 0x06 | — | Reboot |

### Planned opcodes (v2+)

| Op | Code | Meaning |
|----|------|---------|
| JMP | 0x10 | Unconditional jump |
| JZ / JNZ | 0x11 / 0x12 | Conditional |
| SET | 0x20 | Set register / var |
| ADD / SUB | 0x21 / 0x22 | Arithmetic |
| KEY | 0x30 | Wait key |
| DRAW | 0x31 | Draw at x,y |
| COLOR | 0x32 | Set text color |
| CALL_HOST | 0x40 | Host service (fs/net later) |

Only Omni10 understands this bytecode.

---

## SLR source (on SD)

```slr
# comment
print "Hello"
wait 500
info
clear
print "Done"
```

Compiled **on console** to binary `.o10`.

---

## Engine API (`include/o10.h`)

```c
o10_init / o10_deinit
o10_run_file(path)      // SD path when FAT ready
o10_run_string(text)    // text mode
o10_run_blob(data, n)   // binary O10
o10_slr_compile(...)    // on-device SLR → blob
o10_is_script(path)
```

See `source/o10.c` for scaffolding.
