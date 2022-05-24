#include <stdint.h>

#include "uart.h"
#include "gic.h"

const uintptr_t uart0_ptr = (uintptr_t) 0x09000000;
const uintptr_t gicd_ptr = (uintptr_t) 0x08000000;
const uintptr_t gicc_ptr = (uintptr_t) 0x08010000; 

gicd *dist = (gicd*) gicd_ptr; 
gicc *cpu = (gicc*) gicc_ptr;
uart *u = (uart*) uart0_ptr;

#define SGI_INTERRUPT 8
#define TIMER_INTERRUPT 27
#define UART_INTERRUPT 45

int kmain(void) {
    uart_init(u);

    gicd_init(dist);
    gicc_init(cpu);

    gicd_clear_pending(dist, SGI_INTERRUPT);
    gicd_set_priority(dist, SGI_INTERRUPT, 0x00);
    gicd_set_config(dist, SGI_INTERRUPT, 1);
    gicd_enable_irq(dist, SGI_INTERRUPT);

    gicd_sgi(dist, SGI_INTERRUPT);

    return 0;
}

void handle_irq() {
    uint16_t id = gicc_ia(cpu);
    if (id & GICC_IAR_ID_MASK == GIC_SPURIOUS_INTERRUPT) {
        return;
    }

    if (id == SGI_INTERRUPT) {
        uart_puts(u, "SGI IRQ interrupt!!\n");
        uart_putchar(u, (char)id + 0x30);
        gicc_eoi(cpu, id);
    } else if (id == TIMER_INTERRUPT) {
        uart_puts(u, "Timer IRQ interrupt!!\n");
    } else if (id == UART_INTERRUPT) {
        // uart
    } else {
        uart_puts(u, "Unhandled IRQ!!\n");
    }

    // char buf[16];
    // uart_gets(u, buf);
    // uart_puts(u, buf);
}
