#include "gic.h"

const uintptr_t gicd_ptr = (uintptr_t) 0x08000000;
const uintptr_t gicc_ptr = (uintptr_t) 0x08010000; 

gicd *gic_dist = (gicd*) gicd_ptr; 
gicc *gic_cpu = (gicc*) gicc_ptr;

void gicd_init(gicd *dist) {
    // Distributor is disabled on startup, but disable just in case
    dist->ctlr = GICD_CTRL_DISABLE;

    // Clear enable bit for every interrupt (0-1019)
    for (int i = 0; i < 32; i++) {
        dist->icenabler[i] = 0xffffffff;
    }

    // Clear pending bit for every interrupt (0-1019)
    for (int i = 0; i < 32; i++) {
        dist->icpendr[i] = 0xffffffff;
    }

    // Set priority for each interrupt
    for (int i = 0; i < 512; i++) {
        dist->ipriorityr[i] = 0xffffffff;
    }

    // Set target of all interrupts to processor 0
    for (int i = 0; i < 255; i++) {
        dist->itargetsr[i] = 0x01010101;
    }

    // Set configuration of each interrupt, to level-sensitive
   for (int i = 0; i < 128; i++) {
       dist->icfgr[i] = 0x00000000;
   } 

   dist->cpendsgir[3] = 0x01;

    // Enable GICD
    dist->ctlr = GICD_CTRL_ENABLE;
}

void gicd_enable_irq(gicd *dist, irq_id id) {
    // 1 bit per interrupt
    int index = id / 32;
    int bit = id % 32;
    dist->isenabler[index] = (1 << bit);
}

void gicd_disable_irq(gicd *dist, irq_id id) {
    // 1 bit per interrupt
    int index = id / 32;
    int bit = id % 32;
    dist->icenabler[index] = (1 << bit);
}

void gicd_clear_pending(gicd *dist, irq_id id) {
    dist->icpendr[id / 32] = 1U << (id % 32);
}

void gicd_set_priority(gicd *dist, irq_id id, uint8_t prio) {
    // 1 byte (8 bits) per interrupt
    int regn = id / 4;
    int regi = id % 4;
    int shift = regi * 8;
    uint32_t reg = dist->ipriorityr[regn];
    reg &= ~(((uint32_t) 0xff) << shift);
    reg |= prio << shift;
    dist->ipriorityr[regn] = reg;
}

// Configures level-sensitivity/edge-triggering mode for each interrupt
void gicd_set_config(gicd *dist, irq_id id, int config) {
    // 2 bits per interrupt
    int regn = id / 16;
    int regi = id % 16;
    int shift = regi * 2;
    uint32_t reg = dist->icfgr[regn];
    reg &= ~(((uint32_t) 0x2) << shift);
    reg |= (config << 1) << shift;
    dist->icfgr[regn] = reg;
}

void gicd_sgi(gicd *dist, irq_id id) {
    // send to cpu 0, check interrupt 0 <= i < 16
    dist->sgir = (uint32_t) 65536 + 8;
}

void gicc_init(gicc *cpu) {
    cpu->ctlr = GICC_CTRL_DISABLE;

    // Filter lowest priority
    cpu->pmr = 0xff;

    // Single group
    cpu->bpr = 0x0;

    // TODO: clear all interrupts

    cpu->ctlr = GICC_CTRL_ENABLE;
}

uint32_t gicc_acknowledge_interrupt(gicc *cpu) {
    return cpu->iar;
}

void gicc_end_interrupt(gicc *cpu, uint16_t id) {
    cpu->eoir = (uint32_t) id;
}
