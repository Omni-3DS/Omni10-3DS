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

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    // Initialize core systems
    ui_init();
    fs_init();
    net_init();
    o10_init();

    // TODO: Show splash screen from assets/splash/
    // TODO: Mount SD + NAND
    // TODO: Start main UI loop or script runner

    ui_echo("Omni10-3DS " OMNI10_VERSION_STRING);
    ui_echo("Full Access. No Limits.");

    // Placeholder – real main loop comes later
    // while (true) { ui_handle_input(); ... }

    // Cleanup
    o10_deinit();
    net_deinit();
    fs_deinit();
    ui_deinit();

    return 0;
}
