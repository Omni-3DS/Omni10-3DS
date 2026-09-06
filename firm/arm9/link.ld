OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_start)

MEMORY
{
    /* ARM9 WRAM — allow larger FIRM payload (SDMMC+FAT32) */
    ram (rwx) : ORIGIN = 0x08000040, LENGTH = 512K - 0x40
}

SECTIONS
{
    .text :
    {
        KEEP(*(.text.startup))
        *(.text*)
        *(.rodata*)
        . = ALIGN(4);
    } > ram

    .data :
    {
        *(.data*)
        . = ALIGN(4);
    } > ram

    .bss :
    {
        *(.bss*)
        *(COMMON)
        . = ALIGN(8);
    } > ram

    /DISCARD/ :
    {
        *(.comment)
        *(.ARM.attributes)
    }
}
