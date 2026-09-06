/*
 * Omni10 minimal FAT32 — real BPB parse, root dir list, cluster read,
 * and O10BK1 backup in the last 2048 sectors of the volume.
 */
#include "fat32.h"

static void mem_clr(void *p, u32 n)
{
    u8 *b = (u8 *)p;
    while (n--)
        *b++ = 0;
}

static void mem_cpy(void *d, const void *s, u32 n)
{
    u8 *dd = (u8 *)d;
    const u8 *ss = (const u8 *)s;
    while (n--)
        *dd++ = *ss++;
}

static u16 rd16(const u8 *p)
{
    return (u16)p[0] | ((u16)p[1] << 8);
}

static u32 rd32(const u8 *p)
{
    return (u32)p[0] | ((u32)p[1] << 8) | ((u32)p[2] << 16) | ((u32)p[3] << 24);
}

static void wr32(u8 *p, u32 v)
{
    p[0] = (u8)(v);
    p[1] = (u8)(v >> 8);
    p[2] = (u8)(v >> 16);
    p[3] = (u8)(v >> 24);
}

static int read_sec(u32 lba, u8 *buf)
{
    return sdmmc_readsectors(lba, 1, buf);
}

static int write_sec(u32 lba, const u8 *buf)
{
    return sdmmc_writesectors(lba, 1, buf);
}

int fat32_mount(fat32_fs_t *fs)
{
    u8 sec[512];
    u32 root_ents, fatsz16, tot16, tot32, fatsz32, hid;
    u8 part_type;
    u32 part_lba = 0;

    mem_clr(fs, sizeof(*fs));
    if (sdmmc_init() != 0)
        return -1;
    if (read_sec(0, sec) != 0)
        return -2;

    /* MBR? */
    if (sec[510] == 0x55 && sec[511] == 0xAA) {
        part_type = sec[0x1BE + 4];
        part_lba = rd32(&sec[0x1BE + 8]);
        if (part_type == 0x0B || part_type == 0x0C || part_type == 0x0E) {
            if (read_sec(part_lba, sec) != 0)
                return -3;
        } else {
            part_lba = 0; /* may already be VBR */
        }
    }

    if (rd16(&sec[11]) != 512 && rd16(&sec[11]) != 0)
        fs->bytes_per_sec = rd16(&sec[11]);
    else
        fs->bytes_per_sec = 512;

    fs->sec_per_clus = sec[13];
    if (fs->sec_per_clus == 0)
        return -4;
    fs->reserved_sec = rd16(&sec[14]);
    fs->num_fats = sec[16];
    root_ents = rd16(&sec[17]);
    tot16 = rd16(&sec[19]);
    fatsz16 = rd16(&sec[22]);
    tot32 = rd32(&sec[32]);
    fatsz32 = rd32(&sec[36]);
    fs->root_clus = rd32(&sec[44]);
    hid = rd32(&sec[28]);
    (void)hid;

    fs->fat_sz = fatsz16 ? fatsz16 : fatsz32;
    fs->total_sec = tot16 ? tot16 : tot32;
    if (fs->fat_sz == 0 || fs->total_sec == 0)
        return -5;

    fs->fat_begin = part_lba + fs->reserved_sec;
    {
        u32 root_dir_secs = ((root_ents * 32) + (fs->bytes_per_sec - 1)) / fs->bytes_per_sec;
        fs->first_data_sec = fs->fat_begin + (fs->num_fats * fs->fat_sz) + root_dir_secs;
    }
    if (fs->root_clus < 2)
        fs->root_clus = 2;

    fs->mounted = 1;
    return 0;
}

static u32 clus_to_lba(fat32_fs_t *fs, u32 clus)
{
    return fs->first_data_sec + (clus - 2) * fs->sec_per_clus;
}

static u32 fat_next(fat32_fs_t *fs, u32 clus)
{
    u8 sec[512];
    u32 off = clus * 4;
    u32 lba = fs->fat_begin + (off / 512);
    u32 idx = off % 512;
    if (read_sec(lba, sec) != 0)
        return 0x0FFFFFFFu;
    return rd32(&sec[idx]) & 0x0FFFFFFFu;
}

static void name83(const u8 *dirent, char *out)
{
    int i, j = 0;
    for (i = 0; i < 8 && dirent[i] != ' '; i++)
        out[j++] = (char)dirent[i];
    if (dirent[8] != ' ') {
        out[j++] = '.';
        for (i = 8; i < 11 && dirent[i] != ' '; i++)
            out[j++] = (char)dirent[i];
    }
    out[j] = 0;
}

int fat32_list_root(fat32_fs_t *fs, fat32_entry_t *out, int max_out)
{
    u8 sec[512];
    u32 clus = fs->root_clus;
    int n = 0;
    if (!fs->mounted || max_out <= 0)
        return -1;

    while (clus < 0x0FFFFFF8u && n < max_out) {
        u32 s;
        for (s = 0; s < fs->sec_per_clus && n < max_out; s++) {
            int e;
            if (read_sec(clus_to_lba(fs, clus) + s, sec) != 0)
                return -2;
            for (e = 0; e < 16 && n < max_out; e++) {
                u8 *de = &sec[e * 32];
                if (de[0] == 0x00)
                    return n;
                if (de[0] == 0xE5 || (de[11] & 0x08))
                    continue; /* free or volume label */
                if (de[11] & 0x0F) /* LFN skip */
                    continue;
                name83(de, out[n].name);
                out[n].attr = de[11];
                out[n].is_dir = (de[11] & 0x10) ? 1 : 0;
                out[n].clus = ((u32)rd16(&de[20]) << 16) | rd16(&de[26]);
                out[n].size = rd32(&de[28]);
                n++;
            }
        }
        clus = fat_next(fs, clus);
    }
    return n;
}

int fat32_read_file(fat32_fs_t *fs, u32 start_clus, u32 size, u8 *buf, u32 buf_max)
{
    u32 clus = start_clus;
    u32 left = size < buf_max ? size : buf_max;
    u32 got = 0;
    u8 sec[512];

    if (!fs->mounted || !buf)
        return -1;

    while (clus < 0x0FFFFFF8u && left > 0) {
        u32 s;
        for (s = 0; s < fs->sec_per_clus && left > 0; s++) {
            u32 take = left < 512 ? left : 512;
            if (read_sec(clus_to_lba(fs, clus) + s, sec) != 0)
                return -2;
            mem_cpy(buf + got, sec, take);
            got += take;
            left -= take;
        }
        clus = fat_next(fs, clus);
    }
    return (int)got;
}

#define O10_BK_MAGIC 0x314B4230u /* 'O10B' little mixed — store ASCII O10BK1 */
#define O10_TAIL_SECS 2048u

int o10_backup_write(const u8 *payload, u32 len, u32 *out_lba)
{
    fat32_fs_t fs;
    u8 sec[512];
    u32 base, need, i;

    if (fat32_mount(&fs) != 0)
        return -1;
    if (fs.total_sec <= O10_TAIL_SECS + 64)
        return -2;

    base = fs.total_sec - O10_TAIL_SECS;
    /* sector 0 of region: header */
    mem_clr(sec, 512);
    sec[0] = 'O';
    sec[1] = '1';
    sec[2] = '0';
    sec[3] = 'B';
    sec[4] = 'K';
    sec[5] = '1';
    wr32(&sec[8], len);
    wr32(&sec[12], base);
    if (write_sec(base, sec) != 0)
        return -3;

    need = (len + 511) / 512;
    if (need > O10_TAIL_SECS - 1)
        need = O10_TAIL_SECS - 1;

    for (i = 0; i < need; i++) {
        mem_clr(sec, 512);
        {
            u32 off = i * 512;
            u32 chunk = (len - off) > 512 ? 512 : (len - off);
            if (off < len)
                mem_cpy(sec, payload + off, chunk);
        }
        if (write_sec(base + 1 + i, sec) != 0)
            return -4;
    }
    if (out_lba)
        *out_lba = base;
    return 0;
}

int o10_backup_read(u8 *payload, u32 max_len, u32 *out_len)
{
    fat32_fs_t fs;
    u8 sec[512];
    u32 base, len, need, i;

    if (fat32_mount(&fs) != 0)
        return -1;
    if (fs.total_sec <= O10_TAIL_SECS + 64)
        return -2;
    base = fs.total_sec - O10_TAIL_SECS;
    if (read_sec(base, sec) != 0)
        return -3;
    if (sec[0] != 'O' || sec[1] != '1' || sec[2] != '0' || sec[3] != 'B')
        return -4;
    len = rd32(&sec[8]);
    if (len > max_len)
        len = max_len;
    need = (len + 511) / 512;
    for (i = 0; i < need; i++) {
        u32 off = i * 512;
        u32 chunk = (len - off) > 512 ? 512 : (len - off);
        if (read_sec(base + 1 + i, sec) != 0)
            return -5;
        mem_cpy(payload + off, sec, chunk);
    }
    if (out_len)
        *out_len = len;
    return 0;
}
