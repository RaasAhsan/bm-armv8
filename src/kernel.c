#include <stdint.h>

#include "kernel.h"
#include "uart.h"
#include "gic.h"

#include "timer.h"
#include "scheduler.h"
#include "syscall.h"
#include "process.h"
#include "exception.h"

const uintptr_t gicd_ptr = (uintptr_t) 0x08000000;
const uintptr_t gicc_ptr = (uintptr_t) 0x08010000; 

static gicd *dist = (gicd*) gicd_ptr; 
static gicc *cpu = (gicc*) gicc_ptr;

#define INTERRUPT_SGI 8
#define INTERRUPT_TIMER 27
#define INTERRUPT_UART 33

void user_process(void) {
    while (1) {
        syscall(8, 'a', 'b');
        // int a = 2;
        // int b = a + 3;
        // int c = b * a; 
    }
}


void process_a(void) {
    while (1) {
        __asm ("msr daifset, #0b0011");
        timer_sleep(10000);
        uart_putchar('A');
        __asm ("msr daifclr, #0b0011");
        // timer_sleep(1000000);
    }
}

void process_b(void) {
    while (1) {
        __asm ("msr daifset, #0b0011");
        timer_sleep(10000);
        uart_putchar('B');
        __asm ("msr daifclr, #0b0011");
        // timer_sleep(1000000);
    }
}

int kmain(void) {
    uart_init();

    uart_puts("Initialized UART driver...\r\n");

    gicd_init(dist);
    gicc_init(cpu);

    uart_puts("Initialized Generic Interrupt Controller...\r\n");

    gicd_clear_pending(dist, INTERRUPT_SGI);
    gicd_set_priority(dist, INTERRUPT_SGI, 0x00);
    gicd_set_config(dist, INTERRUPT_SGI, GICD_EDGE_TRIGGERED);
    gicd_enable_irq(dist, INTERRUPT_SGI);

    uart_puts("Registered SGI interrupt...\r\n");

    // gicd_sgi(dist, SGI_INTERRUPT);

    gicd_clear_pending(dist, INTERRUPT_UART);
    gicd_set_priority(dist, INTERRUPT_UART, 0x00);
    gicd_set_config(dist, INTERRUPT_UART, GICD_EDGE_TRIGGERED);
    gicd_enable_irq(dist, INTERRUPT_UART);

    uart_puts("Registered UART interrupt...\r\n");

    gicd_clear_pending(dist, INTERRUPT_TIMER);
    gicd_set_priority(dist, INTERRUPT_TIMER, 0x00);
    gicd_set_config(dist, INTERRUPT_TIMER, GICD_LEVEL_SENSITIVE);
    gicd_enable_irq(dist, INTERRUPT_TIMER);

    uart_puts("Initialized timer...\r\n");

    // timer_sleep(5000000);

    uart_puts("Initialization complete!\r\n");

    // scheduler_create_process(process_a);
    // scheduler_create_process(process_b);
    // scheduler_create_process(process_a);

    return 0;
}

// ISR (interrupt service routine) for peripherals
void irq_handler() {
    uint16_t id = gicc_acknowledge_interrupt(cpu);
    if ((id & GICC_IAR_ID_MASK) == GIC_SPURIOUS_INTERRUPT) {
        return;
    }

    if (id == INTERRUPT_SGI) {
        uart_puts("SGI IRQ interrupt!!\n");
        uart_putchar((char)id + 0x30);
        gicc_end_interrupt(cpu, id);
    } else if (id == INTERRUPT_TIMER) {
        // uart_puts("Timer IRQ interrupt!!\n");
        timer_reset();
        gicc_end_interrupt(cpu, id);

        scheduler_switch_process();
    } else if (id == INTERRUPT_UART) {
        uart_puts("UART interrupt!!\n");
        // TODO: handle statuses here
        char c = uart_getchar();
        uart_putchar(c);
        // TODO: this doesn't seem to be necessary
        uart_clear_interrupts();
        gicc_end_interrupt(cpu, id);
    } else {
        uart_puts("Unhandled IRQ!!\n");
        gicc_end_interrupt(cpu, id);
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
            uart_putchar((char) process_get_trap_frame()->x0);
        }
    }
}
