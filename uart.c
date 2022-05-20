#include <stdint.h>
#include "uart.h"

uart* uart_init(uint8_t *address) {
    uart* u = (uart*) address;
    return u;
}

void uart_putchar(uart *u, char c) {
    u->dr = c;
}

void uart_puts(uart *u, const char *s) {
    while (*s != '\0') {
        uart_putchar(u, *s);
        s++;
    }
}
