#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>

typedef enum process_status {
    CREATED,
    PAUSED,
    RUNNING,
    EXITED
} process_status;

// Stores information loaded onto the kernel stack by enter_trap.
typedef struct trap_frame {
    uint64_t x0;
    uint64_t x1;
    uint64_t x2;
    uint64_t x3;
    uint64_t x4;
    uint64_t x5;
    uint64_t x6;
    uint64_t x7;
    uint64_t x8;
    uint64_t x9;
    uint64_t x10;
    uint64_t x11;
    uint64_t x12;
    uint64_t x13;
    uint64_t x14;
    uint64_t x15;
    uint64_t x16;
    uint64_t x17;
    uint64_t x18;
    uint64_t x30;
} trap_frame;

typedef struct process {
    uint8_t pid;
    process_status status;
    uintptr_t program_counter;
    uint8_t* stack;
    trap_frame* trapframe;
} process;

trap_frame* process_get_trap_frame(void);
void process_set_trap_frame(uintptr_t);
void process_set_stack(uintptr_t);

#endif