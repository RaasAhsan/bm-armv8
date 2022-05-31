#ifndef VIRTIO_H
#define VIRTIO_H

#include <stdint.h>

#define VIRTIO_MAGIC 0x74726976
#define VIRTIO_VERSION 0x1
#define VIRTIO_DEVICE_LENGTH 0x200
#define VIRTIO_PAGE_SIZE 4096

#define VIRTIO_DEVICE_INVALID 0
#define VIRTIO_DEVICE_NETWORK 1
#define VIRTIO_DEVICE_BLOCK 2
#define VIRTIO_DEVICE_ENTROPY 4

#define VIRTIO_STATUS_RESET 0
#define VIRTIO_STATUS_ACKNOWLEDGE 1
#define VIRTIO_STATUS_DRIVER 2
#define VIRTIO_STATUS_FAILED 128
#define VIRTIO_STATUS_FEATURES_OK 8 // Unsupported in legacy
#define VIRTIO_STATUS_DRIVER_OK 4
#define VIRTIO_STATUS_DEVICE_NEEDS_RESET 64

typedef volatile struct __attribute__((__packed__)) {
    const uint32_t magic;
    const uint32_t version;
    const uint32_t device_id;
    const uint32_t vendor_id;
    const uint32_t host_features;
    uint32_t host_features_sel;
    const uint32_t _reserved0[2];
    uint32_t guest_features;
    uint32_t guest_features_sel;
    uint32_t guest_page_size;
    const uint32_t _reserved1;
    uint32_t queue_sel;
    const uint32_t queue_num_max;
    uint32_t queue_num;
    uint32_t queue_align;
    uint32_t queue_pfn;
    const uint32_t _reserved2[3];
    uint32_t queue_notify;
    const uint32_t _reserved3[3];
    const uint32_t interrupt_status;
    uint32_t interrupt_ack;
    uint32_t status;
    uint32_t config[0];
} virtio_device;

#define VIRTQ_DESC_NEXT 1
#define VIRTQ_DESC_WRITE 2
#define VIRTQ_DESC_INDIRECT 4

// descriptor for virtq buffers, memory areas where data will be read or written
typedef volatile struct __attribute__((__packed__)) {
    uint64_t addr;
    uint32_t len;
    uint16_t flags;
    uint16_t next;
} virtio_virtq_desc;

typedef volatile struct __attribute__((__packed__)) {
    uint16_t flags;
    uint16_t idx;
    uint16_t ring[0];
} virtio_virtq_avail;

typedef volatile struct __attribute__((__packed__)) {
    uint32_t id;
    uint32_t len;
} virtio_virtq_used_elem;

typedef volatile struct __attribute__((__packed__)) {
    uint16_t flags;
    uint16_t idx;
    virtio_virtq_used_elem ring[0];
} virtio_virtq_used;

typedef struct __attribute__((__packed__)) {
    uintptr_t base;
    virtio_virtq_desc *desc;
    virtio_virtq_avail *avail;
    virtio_virtq_used *used;
} virtio_virtq;

void virtio_init(uintptr_t addr_base, uint16_t irq_base, uint16_t num_devices);
void virtio_virtq_init(virtio_virtq *q, uint16_t queue_size);

#endif