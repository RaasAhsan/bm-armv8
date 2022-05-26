#include <stdint.h>
#include "uart.h"

#define UART_DR_DATA 0xff

void hextochar(char *buf, uint8_t in) {
    uint8_t left = (in >> 4) & 0xf;
    if (left <= 9)  {
        buf[0] = 48 + left;
    } else {
        buf[0] = 65 + left - 10;
    }
    uint8_t right = in & 0xf;
    if (right <= 9)  {
        buf[1] = 48 + right;
    } else {
        buf[1] = 65 + right - 10;
    }
    buf[2] = '\0';
}

void uart_gets(char *buf) {
    char c = uart_getchar();
    while (c != '\n') {
        uart_putchar('A');
        *buf = c;
        buf++;
        c = uart_getchar();
    }
    *buf = '\0';
    uart_puts("\r\n");
}

void uart_puts(const char *s) {
    while (*s != '\0') {
        uart_putchar(*s);
        s++;
    }
}
