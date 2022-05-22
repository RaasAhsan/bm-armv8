#include <stdint.h>

#include "uart.h"

uint8_t *uart0 = (uint8_t *) 0x09000000;
 
void kmain(void) {
    int x = 5 / 0;

    // uart* u = uart_init(uart0);
    // uart_puts(u, "Helo worlds!\n");
    // char buf[16];
    // uart_gets(u, buf);
    // uart_puts(u, buf);
}