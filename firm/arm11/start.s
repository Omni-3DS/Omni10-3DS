/* Omni10 ARM11 entry stub */
.section .text.boot, "ax", %progbits
.global _start
.arm
_start:
1:  wfi
    b 1b
