/*
 * Omni10-3DS – Splash Screen
 *
 * Official design:
 * - Pure black background
 * - Bold pixel-style white text "Omni10-3DS"
 * - Gray subtitle "Full Access. No Limits."
 * - Classic GodMode9 aesthetic
 */

#include "splash.h"

omni_result_t splash_init(void)
{
    // TODO: Load splash.png from data/ or embedded VRAM
    return OMNI_OK;
}

void splash_deinit(void)
{
}

omni_result_t splash_show(void)
{
    // TODO:
    // 1. Clear screens to black
    // 2. Draw splash.png centered on top screen (400x240)
    // 3. Optionally draw bottom screen content
    // 4. Wait ~1 second or until key press (like GodMode9)

    return OMNI_OK;
}

omni_result_t splash_show_timed(int duration_ms)
{
    (void)duration_ms;
    return splash_show();
}
