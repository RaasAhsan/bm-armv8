#include <stdint.h>

#include "uart.h"
#include "gic.h"

uintptr_t uart0_ptr = (uintptr_t) 0x09000000;
uintptr_t gicd_ptr = (uintptr_t) 0x08000000;
uintptr_t gicc_ptr = (uintptr_t) 0x08010000; 

int kmain(void) {
    gicd *dist = (gicd*) gicd_ptr;
    gicc *cpu = (gicc*) gicc_ptr;

    gicd_init(dist);
    gicc_init(cpu);

    // dist->icpendr[0] = 0x8;

    gicd_clear_pending(dist, 8);
    gicd_set_priority(dist, 8, 0x00);
    gicd_set_config(dist, 8, 1);

    gicd_enable_irq(dist, 8);
    // gicd_enable_irq(dist, 27);

    // dist->icpendr[0] = 8;

    // int ptr = (int) &dist->sgir;
    // return ptr;

    // return sizeof(gicd);

    return 0;
    
    // dist->ipriorityr[6] = 0x0;
    // dist->icpendr[0] = 0xffffffff;

    // uart* u = uart_init(uart0);
    // uart_puts(u, "Helo worlds!\n");
    // char buf[16];
    // uart_gets(u, buf);
    // uart_puts(u, buf);
}

void bmain(void) {
    gicd *dist = (gicd*) gicd_ptr;
    gicd_sgi(dist, 8);
}