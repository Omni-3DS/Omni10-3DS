# o10 + Mini-Lua + WiFi (FIRM)

Built into `firm/arm9/main.c` (v0.8.6+).

## o10 text commands

```
PRINT text
WAIT n          # n ~ frames
WAIT
CLS
LED n           # MCU wifi LED reg
BAT
WIFI
INFO
REBOOT
POWEROFF
# comment
```

Binary `.o10`: magic `O10\0` then ops `0x01` PRINT… `0x02` WAIT u16 `0x03` REBOOT `0x04` POWEROFF `0x05` CLS `0x06` LED `0x07` BAT `0x08` WIFI `0x09` INFO `0xFF` END.

## Mini-Lua

```lua
print("hello")
wait(200)
info()
led(15)
bat()
wifi()
-- comment
```

Path planned: `sdmc:/o10/lua/` (demo runs from FIRM ROM until FAT is ready).

## WiFi

ARM9 FIRM can read MCU wireless flag / LED (regs `0x61`, `0x2A`).  
**Full TCP/IP stack needs ARM11 + `soc:u`** (homebrew / dual-core path) — not available in pure ARM9 FIRM.

Menu: **INTERNET / WIFI** shows status + “TCP: ARM11 NEEDED”.
