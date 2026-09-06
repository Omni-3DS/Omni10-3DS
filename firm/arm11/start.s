.section .text.startup, "ax", %progbits
.global _start
.type _start, %function
.align 4

_start:
    ldr sp, =_stack_top
    /* invalidate caches lightly */
    mov r0, #0
    mcr p15, 0, r0, c7, c5, 0
    mcr p15, 0, r0, c7, c6, 0
    mcr p15, 0, r0, c7, c10, 4
    bl  main
.hang:
    b   .hang
.size _start, . - _start

.section .bss
.align 3
.space 0x2000
.global _stack_top
_stack_top:
