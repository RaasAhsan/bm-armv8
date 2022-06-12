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
// The trap frame is used by the kernel to preserve part of the process context,
// and is used to return from trap when switching back to a process (or context switching)
struct trap_frame {
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
};

struct process {
    uint8_t pid;
    process_status status;
    uintptr_t program_counter;
    uintptr_t stack_pointer;
    struct trap_frame context;
};

struct process *process_create(void (*)(void));
void process_suspend(struct process*);
void process_resume(struct process*);

// Used during interrupts
struct trap_frame *process_get_trap_frame(void);
void process_set_trap_frame(uintptr_t);

// Used during context switches
void process_restore_context(uintptr_t pc, uintptr_t sp);
void process_save_context(uintptr_t* pc, uintptr_t* sp);

struct process_head {
    struct process *p;
    struct process_head *next;
};

struct process_list {
    struct process_head *head;
    struct process_head *tail;
};

void process_list_init(struct process_list*);
void process_list_push(struct process_list*, struct process*);
struct process *process_list_pop(struct process_list*);

extern struct trap_frame *trapframe;

#endif