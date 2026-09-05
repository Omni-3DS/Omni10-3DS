/*
 * Omni10-3DS — source/main.c (SCAFFOLDING ONLY)
 *
 * *** THIS FILE IS NOT BUILT INTO Omni10.firm ***
 *
 * The real payload is:
 *   firm/arm9/main.c   (~29KB, full menu + HOME SCRIPTS + X + FTP + browser)
 *
 * Build:  make firm
 * See:    source/README.md
 */

#include "common.h"
#include "ui.h"
#include "fs.h"
#include "net.h"
#include "o10.h"
#include "sys.h"
#include "splash.h"
#include "lang.h"

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    /* Scaffolding init — not used by current FIRM Makefile */
    sys_init();
    lang_init();
    ui_init();
    fs_init();
    net_init();
    o10_init();
    splash_init();
    splash_show();

    ui_echo(_("APP_NAME"));
    ui_echo(_("TAGLINE"));
    ui_echo(_("BOOTING"));

    /*
     * TODO (modular port from firm/arm9/main.c):
     *  - main menu loop
     *  - HOME SCRIPTS hub (X button)
     *  - file browser / FTP
     *  - battery / wifi status bar
     */

    while (1) {
        /* idle */
    }
    return 0;
}
