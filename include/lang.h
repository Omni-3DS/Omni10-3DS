#pragma once

#include "common.h"

/*
 * Omni10-3DS Multi-Language System
 *
 * Supported languages:
 *   EN – English (default)
 *   DE – German
 *   IT – Italian
 *   FR – French
 *   ES – Spanish
 *   NL – Dutch
 *   PT – Portuguese
 *   JA – Japanese (future)
 */

typedef enum {
    LANG_EN = 0,
    LANG_DE,
    LANG_IT,
    LANG_FR,
    LANG_ES,
    LANG_NL,
    LANG_PT,
    LANG_COUNT
} lang_id_t;

omni_result_t lang_init(void);
void lang_deinit(void);

omni_result_t lang_set(lang_id_t id);
lang_id_t lang_get(void);
const char* lang_get_name(lang_id_t id);
const char* lang_get_code(lang_id_t id);

// Get a translated string by key
const char* lang_get_str(const char* key);

// Convenience macro
#define _(key) lang_get_str(key)
