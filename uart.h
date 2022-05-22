#include <stdint.h>

typedef struct {
    volatile uint8_t dr;
    volatile uint32_t rsrecr;
    volatile uint32_t unused_1;
    volatile uint32_t unused_2;
    volatile uint32_t unused_3;
    volatile uint32_t unused_4;
    volatile uint32_t fr;
    volatile uint32_t unused_5;
    volatile uint32_t ilpr;
    volatile uint32_t ibrd;
    volatile uint32_t fbrd;
    volatile uint32_t lcrh;
    volatile uint32_t cr;
    volatile uint32_t ifls;
    volatile uint32_t imsc;
} uart;

uart* uart_init(uint8_t *address);
void uart_putchar(uart *u, char c);
char uart_getchar(uart *u);
void uart_gets(uart *u, char *buf);
void uart_puts(uart *u, const char *s);
