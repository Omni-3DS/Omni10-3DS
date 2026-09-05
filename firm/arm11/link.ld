OUTPUT_FORMAT("elf32-littlearm")
ENTRY(_start)
SECTIONS {
  . = 0x1FF80000;
  .text : { *(.text.boot) *(.text*) }
  .rodata : { *(.rodata*) }
  .data : { *(.data*) }
  .bss : { *(.bss*) *(COMMON) }
}
