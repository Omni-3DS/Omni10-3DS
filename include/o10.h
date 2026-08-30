#pragma once

#include "common.h"

/*
 * Omni10 .o10 Script Engine interface
 */

omni_result_t o10_init(void);
void o10_deinit(void);

// Run a .o10 script from path
omni_result_t o10_run_file(const char* path);

// Run a .o10 script from memory
omni_result_t o10_run_string(const char* script);

// Check if a file is a valid .o10 script
bool o10_is_script(const char* path);
