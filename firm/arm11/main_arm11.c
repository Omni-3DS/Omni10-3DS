/* Omni10 ARM11 — display/GPU assist reserved */
#include <stdint.h>

void arm11_main(void) {
    for (;;) {
        __asm__ volatile("wfi");
    }
}
