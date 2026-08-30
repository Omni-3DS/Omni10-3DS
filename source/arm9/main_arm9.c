/*
 * Omni10-3DS – ARM9 main
 * Low-level: FS, crypto, NAND, IPC to ARM11
 * Original code – not derived from GodMode9
 */

#include "common.h"

void main_arm9(void)
{
    /* TODO:
     * 1. Init hardware (IRQ, timers, SDMMC)
     * 2. Init crypto
     * 3. Mount filesystems
     * 4. Start ARM11
     * 5. IPC loop / script engine host
     */

    while (1) {
        /* idle / IPC */
    }
}
