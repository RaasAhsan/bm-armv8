.macro vector_table_align 
.align 11
.endm

.macro vector_entry_align 
.align 7
.endm

.global _start
_start:
    // Initialize stack
    ldr x30, =stack_top
    mov sp, x30

    // Initialize vector table
    ldr x20, =vectors
    msr vbar_el1, x20

    // Turn off IRQ/FIQ masks
    msr daifclr, #0b0011

    mov x13, 8
    mov x14, 9

    bl kmain

    // ldr x14, =0xdeadbeef

    // Initialize timer
    //mrs x21, cntv_ctl_el0 // read ctl
    //ldr x15, =0x1
    //orr x21, x21, x15 // turn on enable bit
    //msr cntv_ctl_el0, x21 // write ctl

    // ldr x25, =2000
    // msr cntv_tval_el0, x25
    //mrs x17, cntv_tval_el0
    //mrs x18, cntv_cval_el0
    mov x8, 10
foo:
    // mrs x12, SPSel
    mov x10, 8
    mov x10, 9
    // svc #1
    b foo
test:
    // Check system counter, write 1000 to tval, then observe that cval is system counter + 1000
    mrs x23, cntpct_el0
    ldr x25, =1000
    msr cntv_tval_el0, x25
    mrs x24, cntv_cval_el0

vector_table_align
vectors:
    // Current EL with SP0
    vector_entry_align
curr_el_sp0_sync:
    b .
    vector_entry_align
curr_el_sp0_irq:
    b .
    vector_entry_align
curr_el_sp0_fiq:
    b .
    vector_entry_align
curr_el_sp0_serror:
    b .

    // Current EL with SPx
    // brk comes here
    vector_entry_align
curr_el_spx_sync:
    // ldr x25, =0x34
    // exception syndrome register
    mrs x25, esr_el1
    b .
    vector_entry_align
curr_el_spx_irq:
    sub sp, sp, #160 
    stp x0, x1, [sp, #0]
    stp x2, x3, [sp, #16]
    stp x4, x5, [sp, #32]
    stp x6, x7, [sp, #48]
    stp x8, x9, [sp, #64]
    stp x10, x11, [sp, #80]
    stp x12, x13, [sp, #96]
    stp x14, x15, [sp, #112]
    stp x16, x17, [sp, #128]
    stp x18, x30, [sp, #144]

    bl irq_handler

    ldp x0, x1, [sp, #0]
    ldp x2, x3, [sp, #16]
    ldp x4, x5, [sp, #32]
    ldp x6, x7, [sp, #48]
    ldp x8, x9, [sp, #64]
    ldp x10, x11, [sp, #80]
    ldp x12, x13, [sp, #96]
    ldp x14, x15, [sp, #112]
    ldp x16, x17, [sp, #128]
    ldp x18, x30, [sp, #144]
    add sp, sp, #160

    eret
    vector_entry_align
curr_el_spx_fiq:
    b .
    vector_entry_align
curr_el_spx_serror:
    b .

    // Lower EL using AArch64
    vector_entry_align
    b .
    vector_entry_align
    b .
    vector_entry_align
    b .
    vector_entry_align
    b .

    // Lower EL using AArch32
    vector_entry_align
    b .
    vector_entry_align
    b .
    vector_entry_align
    b .
    vector_entry_align
    b .
