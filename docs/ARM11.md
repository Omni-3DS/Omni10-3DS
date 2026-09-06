# Omni10 dual-core FIRM (ARM9 + ARM11)

## Layout

| Core | Load | Copy | Role |
|------|------|------|------|
| ARM9 | `0x08000040` | NDMA | UI, MCU I2C, scripts, R4/DSTT flow |
| ARM11 | `0x1FF80000` | XDMA | SHM IPC, WiFi/FTP **state** |

## Shared memory `0x1FF80E00`

Magic `O10A`. ARM9 can PING / set WiFi flag / FTP start-stop; ARM11 clears `cmd` and sets `status`.

## firmtool

```text
firmtool build Omni10.firm -i \
  -n 0x08000040 -e 0x1FF80000 \
  -D arm9.bin arm11.bin \
  -A 0x08000040 0x1FF80000 \
  -C NDMA XDMA
```

## Honest limits

- **Real** dual-core binary section + IPC loop
- **Not** full Nintendo WiFi driver inside FIRM
- TCP sockets remain a later step (or installer `.3dsx` with `soc:u`)
