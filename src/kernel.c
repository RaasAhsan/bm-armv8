#include <stdint.h>

#include "uart.h"
#include "gic.h"

#include "timer.h"
#include "scheduler.h"
#include "syscall.h"
#include "process.h"
#include "exception.h"
#include "virtio.h"

#include "rstdlib.h"

#define VIRTIO_BASE 0x0a000000
#define VIRTIO_MAX_DEVICES 32

void user_process(void) {
    while (1) {
        for (int i = 0; i < 10000000; i++) {}
        syscall(8, 'A');
        for (int i = 0; i < 10000000; i++) {}
        syscall(8, 'B');
    }
}

void user_process_2(void) {
    while (1) {
        for (int i = 0; i < 10000000; i++) {}
        syscall(8, 'C');
        for (int i = 0; i < 10000000; i++) {}
        syscall(8, 'D');
    }
}

void kernel_init(void) {
    uart_init();

    uart_puts("Initialized UART driver...\r\n");

    gicd_init(gic_dist);
    gicc_init(gic_cpu);

    uart_puts("Initialized Generic Interrupt Controller...\r\n");

    gicd_clear_pending(gic_dist, INTERRUPT_SGI);
    gicd_set_priority(gic_dist, INTERRUPT_SGI, 0x00);
    gicd_set_config(gic_dist, INTERRUPT_SGI, GICD_EDGE_TRIGGERED);
    gicd_enable_irq(gic_dist, INTERRUPT_SGI);

    uart_puts("Registered SGI interrupt...\r\n");

    // gicd_sgi(dist, SGI_INTERRUPT);

    gicd_clear_pending(gic_dist, INTERRUPT_UART);
    gicd_set_priority(gic_dist, INTERRUPT_UART, 0x00);
    gicd_set_config(gic_dist, INTERRUPT_UART, GICD_EDGE_TRIGGERED);
    gicd_enable_irq(gic_dist, INTERRUPT_UART);

    uart_puts("Registered UART interrupt...\r\n");

    gicd_clear_pending(gic_dist, INTERRUPT_TIMER);
    gicd_set_priority(gic_dist, INTERRUPT_TIMER, 0x00);
    gicd_set_config(gic_dist, INTERRUPT_TIMER, GICD_LEVEL_SENSITIVE);
    gicd_enable_irq(gic_dist, INTERRUPT_TIMER);

    uart_puts("Initialized timer...\r\n");

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
