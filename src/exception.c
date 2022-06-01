#include <stdint.h>
#include <stdlib.h>

#include "exception.h"
#include "gic.h"
#include "process.h"
#include "uart.h"
#include "scheduler.h"
#include "timer.h"

#define MAX_INTERRUPTS 1023

static void (*isr[MAX_INTERRUPTS])(void);

void register_isr(uint16_t irq_id, void (*handler)(void)) {
    isr[irq_id] = handler;
}

// ISR (interrupt service routine) for peripherals
void irq_handler() {
    uint16_t id = gicc_acknowledge_interrupt(gic_cpu);
    if ((id & GICC_IAR_ID_MASK) == GIC_SPURIOUS_INTERRUPT) {
        return;
    }

    void (*handler)(void) = isr[id];
    if (handler == NULL) {
        uart_puts("Unhandled IRQ!\n");
        gicc_end_interrupt(gic_cpu, id);
        return;
    }

    handler();

    // if (id == 32 + 16 + 30) {
    //     uart_puts("virtio interrupt!\n");
    // }

    gicc_end_interrupt(gic_cpu, id);

    // char buf[16];
    // uart_gets(u, buf);
    // uart_puts(u, buf);
}

void sync_handler() {
    uint8_t ec = get_exception_class();

    if (ec == EXCEPTION_SVC) {
        long syscall = process_get_trap_frame()->x8;
        if (syscall == 8) {
            char c = (char) process_get_trap_frame()->x0;
            uart_putchar(c);
        }
    } else {
        uart_puts("Encountered an exception.");
    }
}

