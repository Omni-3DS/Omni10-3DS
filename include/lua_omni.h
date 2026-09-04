#pragma once

#include "common.h"

/*
 * Optional Lua embed for Omni10 (planned).
 * Does not replace o10script; shares SD paths under sdmc:/o10/
 */

omni_result_t lua_omni_init(void);
void lua_omni_deinit(void);

omni_result_t lua_omni_run_file(const char *path);
omni_result_t lua_omni_run_string(const char *code);

/* Register host libs: ui, fs, sys, o10 bridge — when VM is linked */
omni_result_t lua_omni_register_host(void);
