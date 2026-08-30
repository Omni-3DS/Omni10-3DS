/*
 * Omni10-3DS – ARM9 entry (skeleton)
 * Own implementation – not based on GodMode9
 */

.section .text.start
.global _start
.align 4

_start:
    @ Clear BSS (placeholder)
    @ Setup stacks
    @ Jump to C main

    b   main_arm9

.global main_arm9
main_arm9:
    @ Will call into C code later
    b   .
