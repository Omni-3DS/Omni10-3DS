/*
 * Omni10-3DS
 * Main entry point
 *
 * Copyright (C) 2026 Omni10 Team
 * Licensed under GPL-3.0
 */

#include "common.h"
#include "ui.h"
#include "fs.h"
#include "net.h"
#include "o10.h"
#include "sys.h"
#include "splash.h"

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    // Initialize core systems
    sys_init();
    ui_init();
    fs_init();
    net_init();
    o10_init();
    splash_init();

    // Show splash screen
    splash_show();

    // TODO: Mount SD + NAND partitions
    // TODO: Check for autorun script (data/autorun.o10 or autorun.lua)
    // TODO: Start main file browser / UI loop

    ui_echo("Omni10-3DS " OMNI10_VERSION_STRING);
    ui_echo("Full Access. No Limits.");

    // Main loop placeholder
    // while (running) {
    //     ui_handle_input();
    //     // file browser, scripts menu, etc.
    // }

    // Cleanup
    splash_deinit();
    o10_deinit();
    net_deinit();
    fs_deinit();
    ui_deinit();
    sys_deinit();

    return 0;
}
