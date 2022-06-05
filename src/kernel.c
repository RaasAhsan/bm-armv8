#include <stdint.h>
#include <stdlib.h>

#include "uart.h"
#include "gic.h"

#include "timer.h"
#include "scheduler.h"
#include "syscall.h"
#include "process.h"
#include "exception.h"
#include "virtio.h"
#include "kmalloc.h"

#include "rstdlib.h"

#define VIRTIO_BASE 0x0a000000
#define VIRTIO_MAX_DEVICES 32

void user_process(void) {
    while (1) {
        for (int i = 0; i < 10000000; i++) {}
        syscall(SYSCALL_UART_OUT, 'A');
        for (int i = 0; i < 10000000; i++) {}
        syscall(SYSCALL_UART_OUT, 'B');
    }
}

void user_process_2(void) {
    while (1) {
        for (int i = 0; i < 10000000; i++) {}
        syscall(SYSCALL_UART_OUT, 'C');
        for (int i = 0; i < 10000000; i++) {}
        syscall(SYSCALL_UART_OUT, 'D');
    }
}

void sgi_handler(void *data) {
    uart_puts("SGI IRQ interrupt!!\n");
}

void timer_handler(void *data) {
    // uart_puts("Timer IRQ interrupt!!\n");
    timer_reset();

    scheduler_switch_process();
}

void uart_handler(void *data) {
    uart_puts("UART interrupt!!\n");
    // TODO: handle statuses here
    char c = uart_getchar();
    uart_putchar(c);
    // TODO: this doesn't seem to be necessary
    uart_clear_interrupts();
}

void kernel_init(void) {
    uart_init();
    uart_puts("Initialized UART driver...\r\n");

    kmalloc_init(0x40800000, 0x20000);
    uart_puts("Initialized kernel memory allocator...\r\n");

    gicd_init(gic_dist);
    gicc_init(gic_cpu);
    uart_puts("Initialized Generic Interrupt Controller...\r\n");

    isr_handler sgi_isr;
    sgi_isr.handler = sgi_handler;
    register_isr(INTERRUPT_SGI, sgi_isr);
    gicd_clear_pending(gic_dist, INTERRUPT_SGI);
    gicd_set_priority(gic_dist, INTERRUPT_SGI, 0x00);
    gicd_set_config(gic_dist, INTERRUPT_SGI, GICD_EDGE_TRIGGERED);
    gicd_enable_irq(gic_dist, INTERRUPT_SGI);

    uart_puts("Registered SGI interrupt...\r\n");

    // gicd_sgi(dist, SGI_INTERRUPT);

    isr_handler uart_isr;
    uart_isr.handler = uart_handler;
    register_isr(INTERRUPT_UART, uart_isr);
    gicd_clear_pending(gic_dist, INTERRUPT_UART);
    gicd_set_priority(gic_dist, INTERRUPT_UART, 0x00);
    gicd_set_config(gic_dist, INTERRUPT_UART, GICD_EDGE_TRIGGERED);
    gicd_enable_irq(gic_dist, INTERRUPT_UART);

    uart_puts("Registered UART interrupt...\r\n");

    isr_handler timer_isr;
    timer_isr.handler = timer_handler;
    register_isr(INTERRUPT_TIMER, timer_isr);
    gicd_clear_pending(gic_dist, INTERRUPT_TIMER);
    gicd_set_priority(gic_dist, INTERRUPT_TIMER, 0x00);
    gicd_set_config(gic_dist, INTERRUPT_TIMER, GICD_LEVEL_SENSITIVE);
    gicd_enable_irq(gic_dist, INTERRUPT_TIMER);

    uart_puts("Initialized timer...\r\n");

    // gicd_clear_pending(gic_dist, 32 + 16 + 30);
    // gicd_set_priority(gic_dist, 32 + 16 + 30, 0x00);
    // gicd_set_config(gic_dist, 32 + 16 + 30, GICD_LEVEL_SENSITIVE);
    // gicd_enable_irq(gic_dist, 32 + 16 + 30);

    // uart_puts("Initialized virtio-rng device...\r\n");

    scheduler_init();

    uart_puts("Initialized scheduler...\r\n");

    virtio_init(VIRTIO_BASE, 32 + 16, VIRTIO_MAX_DEVICES);

    // timer_sleep(5000000);

    uart_puts("Initialization complete!\r\n");
}

int kernel_main(void) {
    scheduler_create_process(user_process);
    scheduler_create_process(user_process_2);

    uart_puts("Started process\r\n");

    scheduler_switch_process();

    return 0;
}
