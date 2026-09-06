# R4 / DSTT flashcart flow (Omni10 FIRM)

## Safety first

Before any flashcart action Omni10 writes a backup:

| Cart | Backup path |
|------|-------------|
| R4 | `sdmc:/o10/r4/backup.bin` |
| DSTT | `sdmc:/o10/dstt/backup.bin` |

Magic header: `O10BK1` (same idea as installer).

## Warning flow

1. **BACKUP** (auto path above) — restore available in same menu
2. Screen: *Please get a magnet and activate sleep mode*
3. Then open browser: `http://<3ds-ip>:1089`
4. Press **A** to continue
5. Notification LED shows **5 colors**, 2 seconds each
6. Order those colors on the companion server to confirm the action

## FIRM limits

- LED sequence uses MCU reg `0x2A` (notification LED pattern)
- Full cart dump/write needs SDMMC + cart bus; UI is ready, low-level dump grows in later builds
- HTTP server on `:1089` needs **ARM11 + real TCP** (same as full WiFi stack)

Menu: **R4 CART** / **DSTT CART**
