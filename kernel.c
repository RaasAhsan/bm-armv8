#include <stdint.h>

#include "uart.h"
#include "gic.h"

uint8_t *uart0_ptr = (uint8_t*) 0x09000000;
uint8_t *gicd_ptr = (uint8_t*) 0x08000000;
uint8_t *gicc_ptr = (uint8_t*) 0x08010000; 

void kmain(void) {
    int x = 5 / 0;

    gicd *dist = (gicd*) gicd_ptr;
    gicc *cpu = (gicc*) gicc_ptr;

    gicd_init(dist);
    gicc_init(cpu);

    // uart* u = uart_init(uart0);
    // uart_puts(u, "Helo worlds!\n");
    // char buf[16];
    // uart_gets(u, buf);
    // uart_puts(u, buf);
}