#pragma once

#include "common.h"

/*
 * Omni10 Splash Screen
 */

omni_result_t splash_init(void);
void splash_deinit(void);

// Show the splash screen (blocks until finished or key pressed)
omni_result_t splash_show(void);

// Show splash for a fixed duration (ms)
omni_result_t splash_show_timed(int duration_ms);
