#ifndef UART_H
#define UART_H
#include <stdint.h>

void uart_init();
void uart_putchar(char c);
char uart_getchar();
void uart_clear_interrupts();

void uart_gets(char *buf);
void uart_puts(const char *s);

#endif