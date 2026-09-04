#pragma once

#include "common.h"
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/* Omni10 o10script / .o10 engine */

#define O10_MAGIC0 'O'
#define O10_MAGIC1 '1'
#define O10_MAGIC2 '0'
#define O10_MAGIC3 0
#define O10_VERSION 1

#define OP_HALT     0x00
#define OP_PRINT    0x01
#define OP_WAIT     0x02
#define OP_CLEAR    0x03
#define OP_INFO     0x04
#define OP_POWEROFF 0x05
#define OP_REBOOT   0x06
/* v2+ */
#define OP_JMP      0x10
#define OP_JZ       0x11
#define OP_JNZ      0x12
#define OP_SET      0x20
#define OP_KEY      0x30

#define O10_CODE_MAX 4096
#define O10_STR_MAX  2048

typedef struct {
    uint8_t code[O10_CODE_MAX];
    int code_len;
    char strtab[O10_STR_MAX];
    int str_len;
    int version;
    int flags;
} o10_prog_t;

omni_result_t o10_init(void);
void o10_deinit(void);

omni_result_t o10_run_file(const char *path);
omni_result_t o10_run_string(const char *script);
omni_result_t o10_run_blob(const uint8_t *data, size_t len);
omni_result_t o10_run_prog(const o10_prog_t *prog);

/* On-device only: compile SLR source text into opaque program */
omni_result_t o10_slr_compile(const char *slr_src, o10_prog_t *out);

bool o10_is_script(const char *path);
bool o10_is_binary(const uint8_t *data, size_t len);
