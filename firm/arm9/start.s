/*
 * Omni10 ARM9 entry
 * Luma chainloader jumps here with:
 *   r0 = argc
 *   r1 = argv
 */

.section .text.startup, "ax", %progbits
.global _start
.type _start, %function
.align 4

_start:
    /* Stack at end of our image RAM */
    ldr sp, =_stack_top

    /* Keep argc/argv for main (r0, r1 already set by Luma) */
    bl  main

.hang:
    b   .hang

.size _start, . - _start

/* 8KB stack in BSS */
.section .bss
.align 3
.space 0x2000
.global _stack_top
_stack_top:
