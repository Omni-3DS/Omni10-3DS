/*
 * Omni10 host-side filesystem backend.
 *
 * All paths are rooted at OMNI_SD_ROOT (default: ./sdmc). The public API
 * accepts paths relative to that root, or sdmc:/... paths. This module is
 * POSIX-backed; the bare-metal FIRM needs its own SDMMC/FAT adapter before
 * this source tree can be linked into firmware.
 */

#define _XOPEN_SOURCE 700
#define _POSIX_C_SOURCE 200809L

#include "fs.h"

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif
#ifndef NAME_MAX
#define NAME_MAX 255
#endif

#define FS_COPY_CHUNK 8192u

static char g_root[PATH_MAX];
static unsigned long g_temp_serial;

static int mkdir_if_missing(const char *path)
{
    struct stat st;
    if (mkdir(path, 0755) == 0)
        return 0;
    if (errno != EEXIST || stat(path, &st) != 0 || !S_ISDIR(st.st_mode))
        return -1;
    return 0;
}

static omni_result_t errno_result(void)
{
    if (errno == ENOENT || errno == ENOTDIR)
        return OMNI_ERR_NOTFOUND;
    if (errno == EACCES || errno == EPERM || errno == EROFS)
        return OMNI_ERR_PERM;
    if (errno == ENOMEM)
        return OMNI_ERR_NOMEM;
    if (errno == EINVAL || errno == ENAMETOOLONG)
        return OMNI_ERR_INVALID;
    return OMNI_ERR_IO;
}

/*
 * Resolve beneath g_root. Parent traversal and existing symlinks are
 * rejected, so caller paths cannot escape the configured root.
 */
static omni_result_t resolve_path(const char *path, int allow_missing_final,
                                  char out[PATH_MAX])
{
    const char *p;
    size_t used;
    char component[NAME_MAX + 1];

    if (!g_root[0] || !path || !*path)
        return OMNI_ERR_INVALID;
    p = path;
    if (!strncasecmp(p, "sdmc:", 5))
        p += 5;
    else if (strchr(p, ':'))
        return OMNI_ERR_INVALID;
    while (*p == '/' || *p == '\\')
        p++;

    used = strlen(g_root);
    if (used + 1 >= PATH_MAX)
        return OMNI_ERR_INVALID;
    memcpy(out, g_root, used + 1);

    while (*p) {
        size_t n = 0;
        struct stat st;
        int final;

        while (*p == '/' || *p == '\\')
            p++;
        if (!*p)
            break;
        while (p[n] && p[n] != '/' && p[n] != '\\') {
            if (n >= NAME_MAX)
                return OMNI_ERR_INVALID;
            component[n] = p[n++];
        }
        component[n] = '\0';
        p += n;
        final = (*p == '\0' || strspn(p, "/\\") == strlen(p));

        if (!strcmp(component, "."))
            continue;
        if (!strcmp(component, "..") || !n)
            return OMNI_ERR_PERM;
        if (used + 1 + n >= PATH_MAX)
            return OMNI_ERR_INVALID;
        out[used++] = '/';
        memcpy(out + used, component, n + 1);
        used += n;

        if (lstat(out, &st) == 0) {
            if (S_ISLNK(st.st_mode))
                return OMNI_ERR_PERM;
            if (!final && !S_ISDIR(st.st_mode))
                return OMNI_ERR_INVALID;
        } else if (errno == ENOENT && final && allow_missing_final) {
            return OMNI_OK;
        } else if (errno == ENOENT) {
            return OMNI_ERR_NOTFOUND;
        } else {
            return errno_result();
        }
    }
    return OMNI_OK;
}

static int name_compare(const void *a, const void *b)
{
    const char *const *sa = (const char *const *)a;
    const char *const *sb = (const char *const *)b;
    return strcasecmp(*sa, *sb);
}

omni_result_t fs_init(void)
{
    const char *configured = getenv("OMNI_SD_ROOT");
    const char *root = (configured && *configured) ? configured : "./sdmc";
    char resolved[PATH_MAX];

    g_root[0] = '\0';
    if (mkdir_if_missing(root) != 0)
        return errno_result();
    if (!realpath(root, resolved))
        return errno_result();
    if (strlen(resolved) >= sizeof(g_root))
        return OMNI_ERR_INVALID;
    strcpy(g_root, resolved);
    g_temp_serial = 0;
    return OMNI_OK;
}

void fs_deinit(void)
{
    g_root[0] = '\0';
    g_temp_serial = 0;
}

bool fs_exists(const char *path)
{
    char resolved[PATH_MAX];
    struct stat st;
    return resolve_path(path, 0, resolved) == OMNI_OK &&
           lstat(resolved, &st) == 0 && !S_ISLNK(st.st_mode);
}

bool fs_is_dir(const char *path)
{
    char resolved[PATH_MAX];
    struct stat st;
    return resolve_path(path, 0, resolved) == OMNI_OK &&
           lstat(resolved, &st) == 0 && S_ISDIR(st.st_mode);
}

omni_result_t fs_list(const char *path, char ***entries, size_t *count)
{
    char resolved[PATH_MAX];
    DIR *dir;
    struct dirent *item;
    char **result = NULL;
    size_t used = 0, capacity = 0;
    omni_result_t status;

    if (!entries || !count)
        return OMNI_ERR_INVALID;
    *entries = NULL;
    *count = 0;
    status = resolve_path(path, 0, resolved);
    if (status != OMNI_OK)
        return status;
    dir = opendir(resolved);
    if (!dir)
        return errno_result();

    errno = 0;
    while ((item = readdir(dir)) != NULL) {
        char **grown;
        char *copy;
        if (!strcmp(item->d_name, ".") || !strcmp(item->d_name, ".."))
            continue;
        if (used == capacity) {
            size_t next = capacity ? capacity * 2 : 8;
            if (next > SIZE_MAX / sizeof(*result)) {
                status = OMNI_ERR_NOMEM;
                goto fail;
            }
            grown = (char **)realloc(result, next * sizeof(*result));
            if (!grown) {
                status = OMNI_ERR_NOMEM;
                goto fail;
            }
            result = grown;
            capacity = next;
        }
        copy = strdup(item->d_name);
        if (!copy) {
            status = OMNI_ERR_NOMEM;
            goto fail;
        }
        result[used++] = copy;
    }
    if (errno != 0) {
        status = errno_result();
        goto fail;
    }
    if (closedir(dir) != 0) {
        status = errno_result();
        dir = NULL;
        goto fail;
    }
    qsort(result, used, sizeof(*result), name_compare);
    *entries = result;
    *count = used;
    return OMNI_OK;

fail:
    if (dir)
        closedir(dir);
    while (used)
        free(result[--used]);
    free(result);
    return status;
}

void fs_list_free(char **entries, size_t count)
{
    size_t i;
    for (i = 0; entries && i < count; i++)
        free(entries[i]);
    free(entries);
}

omni_result_t fs_copy(const char *src, const char *dst)
{
    char source[PATH_MAX], target[PATH_MAX];
    struct stat st;
    unsigned char buffer[FS_COPY_CHUNK];
    FILE *in = NULL, *out = NULL;
    int fd;
    omni_result_t status;

    status = resolve_path(src, 0, source);
    if (status != OMNI_OK)
        return status;
    status = resolve_path(dst, 1, target);
    if (status != OMNI_OK)
        return status;
    if (lstat(source, &st) != 0)
        return errno_result();
    if (!S_ISREG(st.st_mode))
        return OMNI_ERR_INVALID;
    if (lstat(target, &st) == 0)
        return OMNI_ERR_PERM;
    if (errno != ENOENT)
        return errno_result();

    in = fopen(source, "rb");
    if (!in)
        return errno_result();
    fd = open(target, O_WRONLY | O_CREAT | O_EXCL, 0644);
    if (fd < 0) {
        status = errno_result();
        fclose(in);
        return status;
    }
    out = fdopen(fd, "wb");
    if (!out) {
        status = errno_result();
        close(fd);
        unlink(target);
        fclose(in);
        return status;
    }

    status = OMNI_OK;
    for (;;) {
        size_t got = fread(buffer, 1, sizeof(buffer), in);
        if (got && fwrite(buffer, 1, got, out) != got) {
            status = errno_result();
            break;
        }
        if (got < sizeof(buffer)) {
            if (ferror(in))
                status = errno_result();
            break;
        }
    }
    if (fclose(in) != 0 && status == OMNI_OK)
        status = errno_result();
    if (fclose(out) != 0 && status == OMNI_OK)
        status = errno_result();
    if (status != OMNI_OK)
        unlink(target);
    return status;
}

omni_result_t fs_move(const char *src, const char *dst)
{
    char source[PATH_MAX], target[PATH_MAX];
    struct stat st;
    omni_result_t status = resolve_path(src, 0, source);
    if (status != OMNI_OK)
        return status;
    status = resolve_path(dst, 1, target);
    if (status != OMNI_OK)
        return status;
    if (!strcmp(source, g_root))
        return OMNI_ERR_PERM;
    if (lstat(source, &st) != 0)
        return errno_result();
    if (lstat(target, &st) == 0)
        return OMNI_ERR_PERM;
    if (errno != ENOENT)
        return errno_result();
    if (rename(source, target) != 0)
        return errno_result();
    return OMNI_OK;
}

omni_result_t fs_delete(const char *path)
{
    char resolved[PATH_MAX];
    struct stat st;
    omni_result_t status = resolve_path(path, 0, resolved);
    if (status != OMNI_OK)
        return status;
    if (!strcmp(resolved, g_root))
        return OMNI_ERR_PERM;
    if (lstat(resolved, &st) != 0)
        return errno_result();
    if (S_ISDIR(st.st_mode)) {
        if (rmdir(resolved) != 0)
            return errno_result();
    } else if (unlink(resolved) != 0) {
        return errno_result();
    }
    return OMNI_OK;
}

omni_result_t fs_mkdir(const char *path)
{
    char resolved[PATH_MAX];
    struct stat st;
    omni_result_t status = resolve_path(path, 1, resolved);
    if (status != OMNI_OK)
        return status;
    if (!strcmp(resolved, g_root))
        return OMNI_OK;
    if (lstat(resolved, &st) == 0)
        return S_ISDIR(st.st_mode) ? OMNI_OK : OMNI_ERR_INVALID;
    if (errno != ENOENT)
        return errno_result();
    if (mkdir(resolved, 0755) != 0)
        return errno_result();
    return OMNI_OK;
}

omni_result_t fs_read(const char *path, void **data, size_t *size)
{
    char resolved[PATH_MAX];
    struct stat st;
    FILE *file;
    unsigned char *buffer;
    size_t length, offset = 0;
    omni_result_t status;

    if (!data || !size)
        return OMNI_ERR_INVALID;
    *data = NULL;
    *size = 0;
    status = resolve_path(path, 0, resolved);
    if (status != OMNI_OK)
        return status;
    if (stat(resolved, &st) != 0)
        return errno_result();
    if (!S_ISREG(st.st_mode) || st.st_size < 0 ||
        (uintmax_t)st.st_size > (uintmax_t)SIZE_MAX)
        return OMNI_ERR_INVALID;
    length = (size_t)st.st_size;
    buffer = (unsigned char *)malloc(length ? length : 1);
    if (!buffer)
        return OMNI_ERR_NOMEM;
    file = fopen(resolved, "rb");
    if (!file) {
        free(buffer);
        return errno_result();
    }
    while (offset < length) {
        size_t got = fread(buffer + offset, 1, length - offset, file);
        if (!got) {
            status = ferror(file) ? errno_result() : OMNI_ERR_IO;
            fclose(file);
            free(buffer);
            return status;
        }
        offset += got;
    }
    if (fclose(file) != 0) {
        status = errno_result();
        free(buffer);
        return status;
    }
    *data = buffer;
    *size = length;
    return OMNI_OK;
}

void fs_free(void *data)
{
    free(data);
}

omni_result_t fs_write(const char *path, const void *data, size_t size)
{
    char target[PATH_MAX], temp[PATH_MAX];
    struct stat st;
    FILE *file;
    int fd, n;
    omni_result_t status = resolve_path(path, 1, target);

    if (status != OMNI_OK)
        return status;
    if (!data && size)
        return OMNI_ERR_INVALID;
    if (!strcmp(target, g_root))
        return OMNI_ERR_INVALID;
    if (lstat(target, &st) == 0) {
        if (!S_ISREG(st.st_mode))
            return OMNI_ERR_INVALID;
    } else if (errno != ENOENT) {
        return errno_result();
    }

    n = snprintf(temp, sizeof(temp), "%s/.omni-tmp-%ld-%lu", g_root,
                 (long)getpid(), ++g_temp_serial);
    if (n < 0 || (size_t)n >= sizeof(temp))
        return OMNI_ERR_INVALID;
    fd = open(temp, O_WRONLY | O_CREAT | O_EXCL, 0600);
    if (fd < 0)
        return errno_result();
    file = fdopen(fd, "wb");
    if (!file) {
        status = errno_result();
        close(fd);
        unlink(temp);
        return status;
    }
    status = size && fwrite(data, 1, size, file) != size ?
             errno_result() : OMNI_OK;
    if (fclose(file) != 0 && status == OMNI_OK)
        status = errno_result();
    if (status == OMNI_OK && rename(temp, target) != 0)
        status = errno_result();
    if (status != OMNI_OK)
        unlink(temp);
    return status;
}

/* Compact SHA-256 implementation for deterministic file checksums. */
typedef struct {
    uint32_t h[8];
    uint64_t bits;
    unsigned char block[64];
    size_t used;
} sha256_ctx;

static uint32_t rotr32(uint32_t x, unsigned n)
{
    return (x >> n) | (x << (32u - n));
}

static void sha256_block(sha256_ctx *ctx, const unsigned char *block)
{
    static const uint32_t k[64] = {
        0x428a2f98u,0x71374491u,0xb5c0fbcfu,0xe9b5dba5u,0x3956c25bu,0x59f111f1u,0x923f82a4u,0xab1c5ed5u,
        0xd807aa98u,0x12835b01u,0x243185beu,0x550c7dc3u,0x72be5d74u,0x80deb1feu,0x9bdc06a7u,0xc19bf174u,
        0xe49b69c1u,0xefbe4786u,0x0fc19dc6u,0x240ca1ccu,0x2de92c6fu,0x4a7484aau,0x5cb0a9dcu,0x76f988dau,
        0x983e5152u,0xa831c66du,0xb00327c8u,0xbf597fc7u,0xc6e00bf3u,0xd5a79147u,0x06ca6351u,0x14292967u,
        0x27b70a85u,0x2e1b2138u,0x4d2c6dfcu,0x53380d13u,0x650a7354u,0x766a0abbu,0x81c2c92eu,0x92722c85u,
        0xa2bfe8a1u,0xa81a664bu,0xc24b8b70u,0xc76c51a3u,0xd192e819u,0xd6990624u,0xf40e3585u,0x106aa070u,
        0x19a4c116u,0x1e376c08u,0x2748774cu,0x34b0bcb5u,0x391c0cb3u,0x4ed8aa4au,0x5b9cca4fu,0x682e6ff3u,
        0x748f82eeu,0x78a5636fu,0x84c87814u,0x8cc70208u,0x90befffau,0xa4506cebu,0xbef9a3f7u,0xc67178f2u
    };
    uint32_t w[64], a, b, c, d, e, f, g, h;
    unsigned i;
    for (i = 0; i < 16; i++)
        w[i] = ((uint32_t)block[i*4] << 24) | ((uint32_t)block[i*4+1] << 16) |
               ((uint32_t)block[i*4+2] << 8) | (uint32_t)block[i*4+3];
    for (; i < 64; i++) {
        uint32_t x = w[i-15], y = w[i-2];
        uint32_t s0 = rotr32(x,7) ^ rotr32(x,18) ^ (x >> 3);
        uint32_t s1 = rotr32(y,17) ^ rotr32(y,19) ^ (y >> 10);
        w[i] = w[i-16] + s0 + w[i-7] + s1;
    }
    a=ctx->h[0]; b=ctx->h[1]; c=ctx->h[2]; d=ctx->h[3];
    e=ctx->h[4]; f=ctx->h[5]; g=ctx->h[6]; h=ctx->h[7];
    for (i = 0; i < 64; i++) {
        uint32_t s1 = rotr32(e,6) ^ rotr32(e,11) ^ rotr32(e,25);
        uint32_t ch = (e & f) ^ (~e & g);
        uint32_t t1 = h + s1 + ch + k[i] + w[i];
        uint32_t s0 = rotr32(a,2) ^ rotr32(a,13) ^ rotr32(a,22);
        uint32_t maj = (a & b) ^ (a & c) ^ (b & c);
        uint32_t t2 = s0 + maj;
        h=g; g=f; f=e; e=d+t1; d=c; c=b; b=a; a=t1+t2;
    }
    ctx->h[0]+=a; ctx->h[1]+=b; ctx->h[2]+=c; ctx->h[3]+=d;
    ctx->h[4]+=e; ctx->h[5]+=f; ctx->h[6]+=g; ctx->h[7]+=h;
}

static void sha256_update(sha256_ctx *ctx, const unsigned char *data, size_t len)
{
    while (len) {
        size_t take = sizeof(ctx->block) - ctx->used;
        if (take > len)
            take = len;
        memcpy(ctx->block + ctx->used, data, take);
        ctx->used += take;
        ctx->bits += (uint64_t)take * 8u;
        data += take;
        len -= take;
        if (ctx->used == sizeof(ctx->block)) {
            sha256_block(ctx, ctx->block);
            ctx->used = 0;
        }
    }
}

static void sha256_final(sha256_ctx *ctx, unsigned char out[32])
{
    uint64_t bits = ctx->bits;
    unsigned i;
    ctx->block[ctx->used++] = 0x80;
    if (ctx->used > 56) {
        memset(ctx->block + ctx->used, 0, 64 - ctx->used);
        sha256_block(ctx, ctx->block);
        ctx->used = 0;
    }
    memset(ctx->block + ctx->used, 0, 56 - ctx->used);
    for (i = 0; i < 8; i++)
        ctx->block[63-i] = (unsigned char)(bits >> (i*8));
    sha256_block(ctx, ctx->block);
    for (i = 0; i < 8; i++) {
        out[i*4] = (unsigned char)(ctx->h[i] >> 24);
        out[i*4+1] = (unsigned char)(ctx->h[i] >> 16);
        out[i*4+2] = (unsigned char)(ctx->h[i] >> 8);
        out[i*4+3] = (unsigned char)ctx->h[i];
    }
}

omni_result_t fs_hash(const char *path, const char *algo,
                      char *out_hash, size_t out_size)
{
    static const char hex[] = "0123456789abcdef";
    char resolved[PATH_MAX];
    unsigned char digest[32], buffer[FS_COPY_CHUNK];
    size_t i;
    FILE *file;
    struct stat st;
    sha256_ctx ctx = {{0x6a09e667u,0xbb67ae85u,0x3c6ef372u,0xa54ff53au,
                       0x510e527fu,0x9b05688cu,0x1f83d9abu,0x5be0cd19u},0,{0},0};
    omni_result_t status;

    if (!algo || !out_hash || strcasecmp(algo, "sha256"))
        return OMNI_ERR_INVALID;
    if (out_size < 65)
        return OMNI_ERR_INVALID;
    status = resolve_path(path, 0, resolved);
    if (status != OMNI_OK)
        return status;
    if (stat(resolved, &st) != 0)
        return errno_result();
    if (!S_ISREG(st.st_mode))
        return OMNI_ERR_INVALID;
    file = fopen(resolved, "rb");
    if (!file)
        return errno_result();
    for (;;) {
        size_t got = fread(buffer, 1, sizeof(buffer), file);
        if (got)
            sha256_update(&ctx, buffer, got);
        if (got < sizeof(buffer)) {
            if (ferror(file)) {
                status = errno_result();
                fclose(file);
                return status;
            }
            break;
        }
    }
    if (fclose(file) != 0)
        return errno_result();
    sha256_final(&ctx, digest);
    for (i = 0; i < 32; i++) {
        out_hash[i*2] = hex[digest[i] >> 4];
        out_hash[i*2+1] = hex[digest[i] & 15];
    }
    out_hash[64] = '\0';
    return OMNI_OK;
}
