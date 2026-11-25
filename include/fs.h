#pragma once

#include "common.h"

/*
 * Omni10 filesystem API. Paths are relative to the configured SD root or use
 * the sdmc:/ prefix. fs_read and fs_list return heap allocations; release
 * them with fs_free and fs_list_free respectively. fs_hash supports SHA-256.
 */

omni_result_t fs_init(void);
void fs_deinit(void);

bool fs_exists(const char* path);
bool fs_is_dir(const char* path);

omni_result_t fs_list(const char* path, char*** entries, size_t* count);
void fs_list_free(char** entries, size_t count);
omni_result_t fs_copy(const char* src, const char* dst);
omni_result_t fs_move(const char* src, const char* dst);
omni_result_t fs_delete(const char* path);
omni_result_t fs_mkdir(const char* path);

omni_result_t fs_read(const char* path, void** data, size_t* size);
void fs_free(void* data);
omni_result_t fs_write(const char* path, const void* data, size_t size);

omni_result_t fs_hash(const char* path, const char* algo, char* out_hash, size_t out_size);
