.section .text.startup, "ax", %progbits
.global _start
.type _start, %function
.align 4

_start:
    ldr sp, =_stack_top
    mov r4, r0
    mov r5, r1
    mov r0, #0
    mcr p15, 0, r0, c7, c5, 0
    mcr p15, 0, r0, c7, c6, 0
    mcr p15, 0, r0, c7, c10, 4
    mov r0, r4
    mov r1, r5
    bl  main
.hang:
    b   .hang
.size _start, . - _start

.section .bss
.align 3
.space 0x4000
.global _stack_top
_stack_top:
