# Real hardware map (v1.1.0)

## SDMMC (`firm/arm9/sdmmc.c`)

- Base `0x10006000` TMIO
- Init: CMD0, CMD8, ACMD41, CID, RCA, select, blocklen 512
- `sdmmc_readsectors` / `sdmmc_writesectors`

## FAT32 (`firm/arm9/fat32.c`)

- MBR partition or superfloppy VBR
- BPB parse, root cluster walk, 8.3 list, cluster chain read
- **O10BK1** backup: last 2048 sectors of the volume (header + payload)

## R4 / DSTT

UI flow remains; **BACKUP/RESTORE** call `o10_backup_write` / `o10_backup_read` (real SD I/O).

Cart bus dump is a separate layer still expanding.

## WiFi / FTP

MCU flag + ARM11 SHM state = real IPC. Nintendo TCP stack is not part of this FIRM binary.
