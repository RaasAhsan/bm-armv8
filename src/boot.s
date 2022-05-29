.global _start
_start:
    // Initialize kernel stack
    ldr x30, =stack_top
    mov sp, x30

    // Initialize vector table
    ldr x20, =vectors
    msr vbar_el1, x20

    // Initialize timer
    mrs x21, cntv_ctl_el0
    orr x21, x21, #0x00000001 // turn on enable bit
    and x21, x21, #0xfffffffffffffffd // turn off mask bit
    msr cntv_ctl_el0, x21

    // Drop to EL0
    // msr daifclr, #0b0011 // disable irq/fiq mask
    // adr x18, user_process
    // msr elr_el1, x18 // set link after return to user process
    mov x19, xzr // set exception level after return to el0, if to 0
    msr spsr_el1, x19 
    // ldr x20, =0x40202000
    // msr sp_el0, x20 // set stack pointer
    // eret

    bl kernel_init

    sub sp, sp, #160

    // Set the trap frame for the current process
    mov x0, sp // aarch64 calling convention: first arg is in x0
    bl process_set_trap_frame

    bl kernel_main

    eret

