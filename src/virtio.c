#include "virtio.h"
#include "uart.h"
#include "gic.h"
#include "exception.h"

#include <stdint.h>

#define ALIGN(x, align) ((x + (align - 1)) & ~(align - 1))

void virtio_init(uintptr_t addr_base, uint16_t irq_base, uint16_t num_devices) {
    for (int i = 0; i < num_devices; i++) {
        virtio_dev_init(addr_base + VIRTIO_DEVICE_LENGTH * i, irq_base + i);
    }
}

// Legacy initialization sequence: https://docs.oasis-open.org/virtio/virtio/v1.1/csprd01/virtio-v1.1-csprd01.html#x1-940002
void virtio_dev_init(uintptr_t addr, uint16_t irq) {
    virtio_dev *dev = (virtio_dev*) addr;

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
            virtio_rng_init(dev, irq);
        default:
            uart_puts("virtio: unsupported device found\r\n");
            return;
    }
}

#define PAGE_SIZE 4096

static void virtio_rng_isr(void *data) {
    virtio_dev *dev = (virtio_dev*) data;
    uart_puts("virtio-rng interrupt\r\n");

    dev->interrupt_ack = dev->interrupt_status;
}

void virtio_rng_init(virtio_dev *dev, uint16_t irq) {
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

    virtq q;
    virtq_init(&q, qsz);

    dev->queue_num = qsz;
    dev->queue_align = PAGE_SIZE;
    dev->queue_pfn = q.base >> 12;

    isr_handler rng_isr;
    rng_isr.handler = virtio_rng_isr;
    rng_isr.data = (void*) dev;
    register_isr(irq, rng_isr);
    gicd_clear_pending(gic_dist, irq);
    gicd_set_priority(gic_dist, irq, 0x00);
    gicd_set_config(gic_dist, irq, GICD_LEVEL_SENSITIVE);
    gicd_enable_irq(gic_dist, irq);

    dev->status |= VIRTIO_STATUS_DRIVER_OK;

    uart_puts("virtio: entropy device driver ok\r\n");

    uint32_t desc = virtq_desc_alloc(&q);
    q.desc[desc].addr = (uint32_t) (void*) 0x47100000;
    q.desc[desc].len = 32;
    q.desc[desc].flags |= VIRTQ_DESC_WRITE;


    q.avail->ring[q.avail->idx] = desc;
    q.avail->idx += 1;
    dev->queue_notify = 0;
}

void virtq_init(virtq *q, uint16_t queue_size) {
    uintptr_t desc_base = 0x47000000; // already aligned on a new 4096K page
    uint32_t desc_size = sizeof(virtq_desc) * queue_size;
    virtq_desc *desc = (virtq_desc*) desc_base;

    uintptr_t avail_base = desc_base + desc_size; // already aligned on 2 byte boundary
    uint32_t avail_size = sizeof(virtq_avail) + sizeof(uint16_t) * queue_size;
    virtq_avail *avail = (virtq_avail*) avail_base;

    uintptr_t used_base = ALIGN(avail_base + avail_size, PAGE_SIZE);
    virtq_used *used = (virtq_used*) used_base;
    // TODO: we are assuming that there is enough space on this page; in a virtual memory system we would need to allocate

    for (int i = 0; i < queue_size - 1; i++) {
        desc[i].next = i + 1;
    }
    desc[queue_size - 1].next = -1;

    q->base = desc_base;
    q->free_desc = 0;
    q->desc = desc;
    q->avail = avail;
    q->used = used;
}

uint16_t virtq_desc_alloc(virtq *q) {
    uint16_t desc = q->free_desc;
    if (desc == -1) {
        return -1;
    }

    q->free_desc = q->desc[desc].next;

    return desc;
}

void virtq_desc_free(virtq *q, uint16_t desc) {
    q->desc[desc].next = q->free_desc;
    q->free_desc = desc;
}
