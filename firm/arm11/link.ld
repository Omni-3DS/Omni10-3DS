OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_start)

MEMORY
{
    /* AXI WRAM — standard ARM11 FIRM load */
    axiwram (rwx) : ORIGIN = 0x1FF80000, LENGTH = 0x80000
}

SECTIONS
{
    .text :
    {
        KEEP(*(.text.startup))
        *(.text*)
        *(.rodata*)
        . = ALIGN(4);
    } > axiwram

    .data :
    {
        *(.data*)
        . = ALIGN(4);
    } > axiwram

    .bss :
    {
        *(.bss*)
        *(COMMON)
        . = ALIGN(8);
    } > axiwram

    /DISCARD/ :
    {
        *(.comment)
        *(.ARM.attributes)
    }
}
