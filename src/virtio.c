#include "virtio.h"
#include "uart.h"

#include <stdint.h>

void virtio_device_init(uintptr_t addr, uint16_t irq);
void virtio_entropy_init(virtio_device *dev, uint16_t irq);

void virtio_init(uintptr_t addr_base, uint16_t irq_base, uint16_t num_devices) {
    for (int i = 0; i < num_devices; i++) {
        virtio_device_init(addr_base + VIRTIO_DEVICE_LENGTH * i, irq_base + i);
    }
}

void virtio_device_init(uintptr_t addr, uint16_t irq) {
    virtio_device *dev = (virtio_device*) addr;

    if (dev->magic != VIRTIO_MAGIC) {
        uart_puts("virtio: invalid virtio magic\r\n");
        return;
    }

    if (dev->version != VIRTIO_VERSION) {
        uart_puts("virtio: invalid virtio version\r\n");
        return;
    }

    if (dev->device_id == VIRTIO_DEVICE_INVALID) {
        uart_puts("virtio: invalid virtio device found\r\n");
        return;
    }

    dev->status = VIRTIO_STATUS_RESET;
    dev->status |= VIRTIO_STATUS_ACKNOWLEDGE;
    dev->status |= VIRTIO_STATUS_DRIVER;

    switch (dev->device_id) {
        case VIRTIO_DEVICE_ENTROPY:
            virtio_entropy_init(dev, irq);
        default:
            uart_puts("virtio: unsupported device found\r\n");
            return;
    }
}

void virtio_entropy_init(virtio_device *dev, uint16_t irq) {
    uart_puts("virtio: found entropy device\r\n");
}
