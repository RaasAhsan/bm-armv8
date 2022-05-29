#include "scheduler.h"
#include "kernel.h"
#include "uart.h"
#include "exception.h"
#include "process.h"

#define MAX_PROCESSES 128
#define STACK_BASE 0x40200000
#define STACK_SIZE 0x2000

// Process list
static process processes[MAX_PROCESSES];

static int next_process_id = 0;
static int process_count = 0;

static uintptr_t current_stack = STACK_BASE;

void scheduler_init() {
    
}

void scheduler_create_process(void (*start)(void)) {
    if (process_count >= MAX_PROCESSES) {
        return;
    }

    process_count++;

    current_stack += STACK_SIZE;

    process *p = &processes[next_process_id];
    p->pid = (uint8_t) next_process_id;
    p->status = CREATED;
    p->program_counter = (uintptr_t) start;
    p->stack_pointer = (uintptr_t) current_stack;

    current_process = p;

    next_process_id++;
}

// TODO: more sophisticated scheduling
void scheduler_switch_process() {
    if (process_count == 0) {
        return;
    }

    uint8_t current_pid = current_process->pid;
    uint8_t next_pid = (current_pid + 1) % process_count;

    if (next_pid == current_pid) {
        return;
    }

    current_process->context.x0 = trapframe->x0;
    current_process->context.x1 = trapframe->x1;
    current_process->context.x2 = trapframe->x2;
    current_process->context.x3 = trapframe->x3;
    current_process->context.x4 = trapframe->x4;
    current_process->context.x5 = trapframe->x5;
    current_process->context.x6 = trapframe->x6;
    current_process->context.x7 = trapframe->x7;
    current_process->context.x8 = trapframe->x8;
    current_process->context.x30 = trapframe->x30;

    process *next = &processes[next_pid];

    // Set trapframe to current trapframe
    trapframe->x0 = next->context.x0;
    trapframe->x1 = next->context.x1;
    trapframe->x2 = next->context.x2;
    trapframe->x3 = next->context.x3;
    trapframe->x4 = next->context.x4;
    trapframe->x5 = next->context.x5;
    trapframe->x6 = next->context.x6;
    trapframe->x7 = next->context.x7;
    trapframe->x8 = next->context.x8;
    trapframe->x30 = next->context.x30;

    current_process = next;

    process_restore_context(next->program_counter, next->stack_pointer);
}