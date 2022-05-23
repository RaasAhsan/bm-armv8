#include <stdint.h>
#include "uart.h"

// control register bits
#define UART_CR_RXE		(1 << 9)
#define UART_CR_TXE		(1 << 8)
#define UART_CR_UARTEN  (1 << 0)

// flag register bits
#define UART_FR_TXFE	(1 << 7) // transmit fifo empty
#define UART_FR_RXFE	(1 << 4) // receive fifo empty
#define UART_FR_TXFF	(1 << 5) // transmit fifo full

// imsc register bits
#define UART_IMSC_RTIM		(1 << 6)
#define UART_IMSC_RXIM		(1 << 4)

void uart_flush(uart *u) {
    while (!(u->fr & UART_FR_TXFE)) {
        ;
    }
}

void uart_init(uart *u) {
    u->cr = 0x00;

    // Enable interrupts
    u->imsc = UART_IMSC_RXIM | UART_IMSC_RTIM;

    // Enable uart and rx/tx
    u->cr = UART_CR_UARTEN | UART_CR_RXE | UART_CR_TXE;

    uart_flush(u);
}

void uart_putchar(uart *u, char c) {
    u->dr = c;

    while (u->fr & UART_FR_TXFF) {
        ;
    }
}

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

void uart_gets(uart *u, char *buf) {
    char c = uart_getchar(u);
    while (c != '\n') {
        uart_putchar(u, 'A');
        *buf = c;
        buf++;
        c = uart_getchar(u);
    }
    *buf = '\0';
    uart_puts(u, "\r\n");
}

char uart_getchar(uart *u) {
    while (u->fr & UART_FR_RXFE) {
        ;
    }
    char buf[3];
    char c = u->dr & 0xff;
    hextochar(buf, c);
    uart_puts(u, buf);
    return c;
}

void uart_puts(uart *u, const char *s) {
    while (*s != '\0') {
        uart_putchar(u, *s);
        s++;
    }
}
