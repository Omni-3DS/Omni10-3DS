/*
 * Omni10-3DS – Splash Screen (stub)
 */

#include "splash.h"

omni_result_t splash_init(void)
{
    // TODO: Load splash image from data/ or assets/
    return OMNI_OK;
}

void splash_deinit(void)
{
}

omni_result_t splash_show(void)
{
    // TODO: Draw splash on top (and optional bottom) screen
    // TODO: Wait for key or timeout
    return OMNI_OK;
}

omni_result_t splash_show_timed(int duration_ms)
{
    (void)duration_ms;
    return splash_show();
}
