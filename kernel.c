#include <stdint.h>

#include "uart.h"

uint8_t *uart0 = (uint8_t *) 0x09000000;
 
void kmain(void) {
    uart* u = uart_init(uart0);
    uart_puts(u, "Hello worlds!\n");
}