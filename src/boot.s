.global _start
_start:
    // Initialize stack
    ldr x30, =stack_top
    mov sp, x30

    // Initialize vector table
    ldr x20, =vectors
    msr vbar_el1, x20

    bl kmain

    // Initialize timer
    mrs x21, cntv_ctl_el0
    // orr x21, x21, #0x00000001 // turn on enable bit
    and x21, x21, #0xfffffffffffffffd // turn off mask bit
    msr cntv_ctl_el0, x21


    // Drop to EL0
    // msr daifclr, #0b0011 // disable irq/fiq mask
    adr x18, user_process
    msr elr_el1, x18 // set link after return to user process
    mov x19, xzr // set exception level after return to el0, if to 0
    msr spsr_el1, x19 
    ldr x20, =0x40202000
    msr sp_el0, x20 // set stack pointer
    eret

.global reset_timer
reset_timer:
    ldr x25, =62500000
    msr cntv_tval_el0, x25
    ret
