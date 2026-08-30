/*
 * Omni10-3DS – File System implementation (stub)
 */

#include "fs.h"

omni_result_t fs_init(void)
{
    // TODO: Mount SD, NAND partitions, etc.
    return OMNI_OK;
}

void fs_deinit(void)
{
}

bool fs_exists(const char* path)
{
    (void)path;
    return false;
}

bool fs_is_dir(const char* path)
{
    (void)path;
    return false;
}

omni_result_t fs_list(const char* path, char*** entries, size_t* count)
{
    (void)path;
    (void)entries;
    (void)count;
    return OMNI_ERR_GENERIC;
}

omni_result_t fs_copy(const char* src, const char* dst)
{
    (void)src;
    (void)dst;
    return OMNI_ERR_GENERIC;
}

omni_result_t fs_move(const char* src, const char* dst)
{
    (void)src;
    (void)dst;
    return OMNI_ERR_GENERIC;
}

omni_result_t fs_delete(const char* path)
{
    (void)path;
    return OMNI_ERR_GENERIC;
}

omni_result_t fs_mkdir(const char* path)
{
    (void)path;
    return OMNI_ERR_GENERIC;
}

omni_result_t fs_read(const char* path, void** data, size_t* size)
{
    (void)path;
    (void)data;
    (void)size;
    return OMNI_ERR_GENERIC;
}

omni_result_t fs_write(const char* path, const void* data, size_t size)
{
    (void)path;
    (void)data;
    (void)size;
    return OMNI_ERR_GENERIC;
}

omni_result_t fs_hash(const char* path, const char* algo, char* out_hash, size_t out_size)
{
    (void)path;
    (void)algo;
    (void)out_hash;
    (void)out_size;
    return OMNI_ERR_GENERIC;
}
