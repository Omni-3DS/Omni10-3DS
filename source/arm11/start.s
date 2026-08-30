/*
 * Omni10-3DS – ARM11 entry (skeleton)
 * Own implementation – not based on GodMode9
 */

.section .text.start
.global _start_arm11
.align 4

_start_arm11:
    @ Setup MPU / caches (placeholder)
    @ Setup stacks
    @ Jump to C

    b   main_arm11

.global main_arm11
main_arm11:
    b   .
