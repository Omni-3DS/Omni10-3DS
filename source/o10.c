/*
 * Omni10-3DS – o10script engine scaffolding
 * Binary + text runners; SLR compile on-device; SD load when FAT ready.
 */

#include "o10.h"
#include <string.h>

static o10_prog_t g_prog;

omni_result_t o10_init(void)
{
    memset(&g_prog, 0, sizeof(g_prog));
    return OMNI_OK;
}

void o10_deinit(void)
{
    memset(&g_prog, 0, sizeof(g_prog));
}

bool o10_is_script(const char *path)
{
    size_t len;
    if (!path) return false;
    len = strlen(path);
    if (len < 4) return false;
    return strcmp(path + len - 4, ".o10") == 0
        || (len >= 4 && strcmp(path + len - 4, ".slr") == 0);
}

bool o10_is_binary(const uint8_t *data, size_t len)
{
    if (!data || len < 8) return false;
    return data[0] == O10_MAGIC0 && data[1] == O10_MAGIC1
        && data[2] == O10_MAGIC2 && data[3] == O10_MAGIC3;
}

static void emit(o10_prog_t *p, uint8_t b)
{
    if (p->code_len < O10_CODE_MAX)
        p->code[p->code_len++] = b;
}

static void emit16(o10_prog_t *p, unsigned v)
{
    emit(p, (uint8_t)(v & 0xFF));
    emit(p, (uint8_t)((v >> 8) & 0xFF));
}

static int add_str(o10_prog_t *p, const char *s)
{
    int off = p->str_len;
    int n = (int)strlen(s);
    if (p->str_len + n + 1 > O10_STR_MAX) return -1;
    memcpy(p->strtab + p->str_len, s, (size_t)n + 1);
    p->str_len += n + 1;
    return off;
}

static int starts_ci(const char *s, const char *pfx)
{
    while (*pfx) {
        char a = *s, b = *pfx;
        if (a >= 'A' && a <= 'Z') a = (char)(a + 32);
        if (b >= 'A' && b <= 'Z') b = (char)(b + 32);
        if (a != b) return 0;
        s++; pfx++;
    }
    return 1;
}

omni_result_t o10_slr_compile(const char *slr_src, o10_prog_t *out)
{
    char line[128];
    int li = 0;
    if (!slr_src || !out) return OMNI_ERR_GENERIC;
    memset(out, 0, sizeof(*out));
    out->version = O10_VERSION;

    while (1) {
        char c = *slr_src;
        int end = (c == 0);
        if (c == '\n' || end) {
            char *p;
            int n;
            line[li] = 0;
            p = line;
            while (*p == ' ' || *p == '\t') p++;
            n = (int)strlen(p);
            while (n > 0 && (p[n - 1] == ' ' || p[n - 1] == '\t' || p[n - 1] == '\r'))
                p[--n] = 0;
            if (n > 0 && p[0] != '#') {
                if (starts_ci(p, "print ") || starts_ci(p, "print\t")) {
                    char *q = strchr(p, '"');
                    if (q) {
                        char *q2;
                        int off;
                        q++;
                        q2 = strchr(q, '"');
                        if (q2) {
                            *q2 = 0;
                            off = add_str(out, q);
                            if (off < 0) return OMNI_ERR_GENERIC;
                            emit(out, OP_PRINT);
                            emit16(out, (unsigned)off);
                        }
                    }
                } else if (starts_ci(p, "wait")) {
                    const char *nptr = p + 4;
                    unsigned ms = 0;
                    while (*nptr == ' ') nptr++;
                    while (*nptr >= '0' && *nptr <= '9') {
                        ms = ms * 10u + (unsigned)(*nptr - '0');
                        nptr++;
                    }
                    emit(out, OP_WAIT);
                    emit16(out, ms > 0xFFFF ? 0xFFFF : ms);
                } else if (starts_ci(p, "clear")) {
                    emit(out, OP_CLEAR);
                } else if (starts_ci(p, "info")) {
                    emit(out, OP_INFO);
                } else if (starts_ci(p, "poweroff")) {
                    emit(out, OP_POWEROFF);
                } else if (starts_ci(p, "reboot")) {
                    emit(out, OP_REBOOT);
                }
            }
            li = 0;
            if (end) break;
            slr_src++;
            continue;
        }
        if (li < (int)sizeof(line) - 2) line[li++] = c;
        slr_src++;
    }
    emit(out, OP_HALT);
    return OMNI_OK;
}

omni_result_t o10_run_prog(const o10_prog_t *prog)
{
    int ip;
    if (!prog || prog->code_len <= 0) return OMNI_ERR_GENERIC;
    /* Host hooks (draw/power) live in firm/arm9/main.c when integrated */
    ip = 0;
    while (ip < prog->code_len) {
        uint8_t op = prog->code[ip++];
        if (op == OP_HALT) break;
        if (op == OP_PRINT && ip + 1 < prog->code_len) ip += 2;
        else if (op == OP_WAIT && ip + 1 < prog->code_len) ip += 2;
        else if (op == OP_CLEAR || op == OP_INFO) { /* host */ }
        else if (op == OP_POWEROFF || op == OP_REBOOT) { /* host */ break; }
        else { /* unknown: stop */ break; }
    }
    return OMNI_OK;
}

omni_result_t o10_run_blob(const uint8_t *data, size_t len)
{
    unsigned strtab_off;
    unsigned code_len;
    if (!o10_is_binary(data, len)) return OMNI_ERR_GENERIC;
    strtab_off = (unsigned)data[6] | ((unsigned)data[7] << 8);
    if (strtab_off < 8 || strtab_off > len) return OMNI_ERR_GENERIC;
    code_len = strtab_off - 8;
    if (code_len > O10_CODE_MAX) return OMNI_ERR_GENERIC;
    memset(&g_prog, 0, sizeof(g_prog));
    g_prog.version = data[4];
    g_prog.flags = data[5];
    g_prog.code_len = (int)code_len;
    memcpy(g_prog.code, data + 8, code_len);
    g_prog.str_len = (int)(len - strtab_off);
    if (g_prog.str_len > O10_STR_MAX) g_prog.str_len = O10_STR_MAX;
    memcpy(g_prog.strtab, data + strtab_off, (size_t)g_prog.str_len);
    return o10_run_prog(&g_prog);
}

omni_result_t o10_run_string(const char *script)
{
    /* Text mode: compile-as-SLR-like uppercase lines is host-side in main.c demos */
    if (!script) return OMNI_ERR_GENERIC;
    return o10_slr_compile(script, &g_prog);
}

omni_result_t o10_run_file(const char *path)
{
    (void)path;
    /* TODO: SDMMC+FAT read sdmc:/o10/slr/... then o10_run_blob / compile */
    return OMNI_ERR_GENERIC;
}
