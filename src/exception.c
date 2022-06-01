#include <stdint.h>

#include "exception.h"
#include "gic.h"
#include "process.h"
#include "uart.h"
#include "scheduler.h"
#include "timer.h"

// ISR (interrupt service routine) for peripherals
void irq_handler() {
    uint16_t id = gicc_acknowledge_interrupt(gic_cpu);
    if ((id & GICC_IAR_ID_MASK) == GIC_SPURIOUS_INTERRUPT) {
        return;
    }

    if (id == INTERRUPT_SGI) {
        uart_puts("SGI IRQ interrupt!!\n");
        uart_putchar((char)id + 0x30);
        gicc_end_interrupt(gic_cpu, id);
    } else if (id == INTERRUPT_TIMER) {
        // uart_puts("Timer IRQ interrupt!!\n");
        timer_reset();
        gicc_end_interrupt(gic_cpu, id);

        scheduler_switch_process();
    } else if (id == INTERRUPT_UART) {
        uart_puts("UART interrupt!!\n");
        // TODO: handle statuses here
        char c = uart_getchar();
        uart_putchar(c);
        // TODO: this doesn't seem to be necessary
        uart_clear_interrupts();
        gicc_end_interrupt(gic_cpu, id);
    } else if (id == 32 + 16 + 30) {
        uart_puts("virtio interrupt!\n");
        gicc_end_interrupt(gic_cpu, id);
    } else {
        uart_puts("Unhandled IRQ!!\n");
        gicc_end_interrupt(gic_cpu, id);
    }

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

