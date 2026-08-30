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
#include "lang.h"

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    // Initialize core systems
    sys_init();
    lang_init();
    ui_init();
    fs_init();
    net_init();
    o10_init();
    splash_init();

    // Show splash screen
    splash_show();

    // TODO: Mount SD + NAND partitions
    // TODO: Load language from config
    // TODO: Check for autorun script
    // TODO: Start main file browser / UI loop

    ui_echo(_("APP_NAME"));
    ui_echo(_("TAGLINE"));
    ui_echo(_("BOOTING"));

    // Main loop placeholder
    // while (running) {
    //     ui_handle_input();
    // }

    // Cleanup
    splash_deinit();
    o10_deinit();
    net_deinit();
    fs_deinit();
    ui_deinit();
    lang_deinit();
    sys_deinit();

    return 0;
}
