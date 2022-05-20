#include <stdint.h>

typedef struct uart {
    volatile uint8_t dr;
    volatile uint32_t rsrecr;
    volatile uint32_t unused_1;
    volatile uint32_t unused_2;
    volatile uint32_t unused_3;
    volatile uint32_t unused_4;
    volatile uint32_t fr;
} uart;

uart* uart_init(uint8_t *address);
void uart_putchar(uart *u, char c);
void uart_puts(uart *u, const char *s);
