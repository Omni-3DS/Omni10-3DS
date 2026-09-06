/*
 * Minimal MBR + sector dump helpers for Omni10.
 * Real SD reads via sdmmc_readsectors — used by file browser / backup UI.
 */
#include "sdmmc.h"

int fat_read_mbr(u8 *sector512)
{
    if (sdmmc_init() != 0)
        return -1;
    return sdmmc_readsectors(0, 1, sector512);
}

int fat_backup_raw(u32 start_sector, u32 num_sectors, u8 *buf)
{
    if (sdmmc_init() != 0)
        return -1;
    return sdmmc_readsectors(start_sector, num_sectors, buf);
}

int fat_write_raw(u32 start_sector, u32 num_sectors, const u8 *buf)
{
    if (sdmmc_init() != 0)
        return -1;
    return sdmmc_writesectors(start_sector, num_sectors, buf);
}
