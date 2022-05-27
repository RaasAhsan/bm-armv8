ENTRY(_start)
SECTIONS {
    . = 0x40080000;
    .startup . : { boot.o(.text) }
    .text : { *(.text) }
    .data : { *(.data) }
    .bss : { *(.bss COMMON) }
    . = ALIGN(8);
    . += 0x100000; /* 4kB of stack memory */
    stack_top = .;
}