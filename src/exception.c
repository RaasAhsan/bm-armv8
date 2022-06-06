#include <stdint.h>
#include <stdlib.h>

#include "exception.h"
#include "gic.h"
#include "process.h"
#include "uart.h"
#include "scheduler.h"
#include "timer.h"
#include "syscall.h"

#define MAX_INTERRUPTS 1023

// TODO: move this to GIC? technically IRQ handling is part of GIC
static isr_handler isr[MAX_INTERRUPTS];

void register_isr(uint16_t irq_id, isr_handler handler) {
    isr[irq_id] = handler;
}

// ISR (interrupt service routine) for peripherals
void irq_handler() {
    uint16_t id = gicc_acknowledge_interrupt(gic_cpu);
    if ((id & GICC_IAR_ID_MASK) == GIC_SPURIOUS_INTERRUPT) {
        return;
    }

    isr_handler handler = isr[id];
    if (handler.handler == NULL) {
        uart_puts("Unhandled IRQ!\n");
        gicc_end_interrupt(gic_cpu, id);
        return;
    }

    handler.handler(handler.data);

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

    switch(ec) {
        case EXCEPTION_SVC: {
            long syscall = process_get_trap_frame()->x8;
            switch (syscall) {
                case SYSCALL_UART_OUT: {
                    char c = (char) process_get_trap_frame()->x0;
                    uart_putchar(c);
                    break;
                }
                case SYSCALL_EXIT: {
                    // Unschedule process
                    scheduler_exit_process();
                    break;
                }
                default: {
                    uart_puts("Unhandled syscall\r\n");
                }
            }
            return;
        }
        default: {
            uart_puts("Encountered an exception.\r\n");
        }
    }
}

