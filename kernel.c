#include <stdint.h>

#include "uart.h"
#include "gic.h"

const uintptr_t uart0_ptr = (uintptr_t) 0x09000000;
const uintptr_t gicd_ptr = (uintptr_t) 0x08000000;
const uintptr_t gicc_ptr = (uintptr_t) 0x08010000; 

gicd *dist = (gicd*) gicd_ptr; 
gicc *cpu = (gicc*) gicc_ptr;
uart *u = (uart*) uart0_ptr;

#define INTERRUPT_SGI 8
#define TIMER_INTERRUPT 27
#define INTERRUPT_UART 33

int kmain(void) {
    uart_init(u);

    gicd_init(dist);
    gicc_init(cpu);

    gicd_clear_pending(dist, INTERRUPT_SGI);
    gicd_set_priority(dist, INTERRUPT_SGI, 0x00);
    gicd_set_config(dist, INTERRUPT_SGI, GICD_EDGE_TRIGGERED);
    gicd_enable_irq(dist, INTERRUPT_SGI);

    // gicd_sgi(dist, SGI_INTERRUPT);

    gicd_clear_pending(dist, INTERRUPT_UART);
    gicd_set_priority(dist, INTERRUPT_UART, 0x00);
    gicd_set_config(dist, INTERRUPT_UART, GICD_EDGE_TRIGGERED);
    gicd_enable_irq(dist, INTERRUPT_UART);

    return 0;
}

void handle_irq() {
    uart_puts(u, "received interrupt!!\n");

    uint16_t id = gicc_ia(cpu);
    if ((id & GICC_IAR_ID_MASK) == GIC_SPURIOUS_INTERRUPT) {
        return;
    }

    if (id == INTERRUPT_SGI) {
        uart_puts(u, "SGI IRQ interrupt!!\n");
        uart_putchar(u, (char)id + 0x30);
    } else if (id == TIMER_INTERRUPT) {
        uart_puts(u, "Timer IRQ interrupt!!\n");
    } else if (id == INTERRUPT_UART) {
        uart_puts(u, "UART interrupt!!\n");
        char c = uart_getchar(u);
        uart_putchar(u, c);
    } else {
        uart_puts(u, "Unhandled IRQ!!\n");
    }

    gicc_eoi(cpu, id);

    // char buf[16];
    // uart_gets(u, buf);
    // uart_puts(u, buf);
}
