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
