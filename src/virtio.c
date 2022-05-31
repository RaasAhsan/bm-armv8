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

// Legacy initialization sequence: https://docs.oasis-open.org/virtio/virtio/v1.1/csprd01/virtio-v1.1-csprd01.html#x1-940002
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

#define PAGE_SIZE 4096

void virtio_entropy_init(virtio_device *dev, uint16_t irq) {
    uart_puts("virtio: discovered entropy device\r\n");

    dev->host_features_sel = 0x0;
    dev->guest_features_sel = 0x0;

    dev->guest_page_size = VIRTIO_PAGE_SIZE;

    dev->queue_sel = 0;

    if (dev->queue_pfn != 0x00) {
        uart_puts("virtio: queue is in use already\r\n");
        return;
    }

    if (dev->queue_num_max == 0x00) {
        uart_puts("virtio: queue is not available\r\n");
        return;
    }

    uint16_t qsz = 16;

    if (dev->queue_num_max < qsz) {
        uart_puts("virtio: queue size too large\r\n");
        return;
    }

    virtio_virtq q;
    virtio_virtq_init(&q, qsz);

    dev->queue_num = qsz;
    dev->guest_page_size = PAGE_SIZE;
    dev->queue_align = PAGE_SIZE;
    dev->queue_pfn = q.base >> 12;

    dev->status |= VIRTIO_STATUS_DRIVER_OK;

    uart_puts("virtio: entropy device driver ok\r\n");
}

uint32_t align(uint32_t size, int alignment) {
    return (size + (alignment - 1)) & ~(alignment - 1);
}

void virtio_virtq_init(virtio_virtq *q, uint16_t queue_size) {
    uintptr_t desc_base = 0x50000000; // already aligned on a new 4096K page
    uint32_t desc_size = sizeof(virtio_virtq_desc) * queue_size;
    virtio_virtq_desc *desc = (virtio_virtq_desc*) desc_base;

    uintptr_t avail_base = desc_base + desc_size; // already aligned on 2 byte boundary
    uint32_t avail_size = sizeof(virtio_virtq_avail) + sizeof(uint16_t) * queue_size;
    virtio_virtq_avail *avail = (virtio_virtq_avail*) avail_base;

    uintptr_t used_base = align(avail_base + avail_size, PAGE_SIZE);
    virtio_virtq_used *used = (virtio_virtq_used*) used_base;
    // TODO: we are assuming that there is enough space on this page; in a virtual memory system we would need to allocate

    q->base = desc_base;
    q->desc = desc;
    q->avail = avail;
    q->used = used;
}
