ENTRY(_start)
SECTIONS {
    . = 0x40080000;
    .startup . : { out/boot.o(.text) }
    .text : { *(.text) }
    .data : { *(.data) }
    .bss : { *(.bss COMMON) }
    . = ALIGN(8);
    . += 0x1000; /* 4kB of stack memory */
    stack_top = .;
}