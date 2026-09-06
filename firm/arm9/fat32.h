#pragma once
#include "sdmmc.h"

typedef struct {
    u32 bytes_per_sec;
    u32 sec_per_clus;
    u32 reserved_sec;
    u32 num_fats;
    u32 fat_sz;
    u32 root_clus;
    u32 first_data_sec;
    u32 total_sec;
    u32 fat_begin;
    int mounted;
} fat32_fs_t;

typedef struct {
    char name[13]; /* 8.3 + NUL */
    u8  attr;
    u32 size;
    u32 clus;
    int is_dir;
} fat32_entry_t;

int  fat32_mount(fat32_fs_t *fs);
int  fat32_list_root(fat32_fs_t *fs, fat32_entry_t *out, int max_out);
int  fat32_read_file(fat32_fs_t *fs, u32 start_clus, u32 size, u8 *buf, u32 buf_max);
/* Writes O10BK1 payload into reserved tail of volume (last 2048 sectors). */
int  o10_backup_write(const u8 *payload, u32 len, u32 *out_lba);
int  o10_backup_read(u8 *payload, u32 max_len, u32 *out_len);
