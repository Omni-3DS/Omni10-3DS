/* Omni10 ARM9 entry – set stack, forward argc/argv from Luma chainloader */

.section .text.startup
.global _start
.align 4

_start:
    ldr sp, =stack_top
    /* r0 = argc, r1 = argv (from Luma) */
    bl  main
1:
    b   1b

.section .bss
.align 4
.space 0x2000
stack_top:
