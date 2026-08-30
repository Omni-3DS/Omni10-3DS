#pragma once

/*
 * Omni10-3DS – Common definitions
 */

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define OMNI10_VERSION_MAJOR 0
#define OMNI10_VERSION_MINOR 1
#define OMNI10_VERSION_PATCH 0
#define OMNI10_VERSION_STRING "0.1.0-dev"

#define OMNI10_NAME "Omni10-3DS"

// Useful macros
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

// Error codes (placeholder)
typedef enum {
    OMNI_OK = 0,
    OMNI_ERR_GENERIC = -1,
    OMNI_ERR_NOMEM = -2,
    OMNI_ERR_NOTFOUND = -3,
    OMNI_ERR_IO = -4,
    OMNI_ERR_PERM = -5,
    OMNI_ERR_INVALID = -6,
} omni_result_t;
