#include <stdlib.h>

#include "scheduler.h"
#include "uart.h"
#include "exception.h"
#include "process.h"

#define MAX_PROCESSES 128
#define PROC_MEM_BASE 0x40200000
#define PROC_MEM_SIZE 0x2000

// Process list
static process processes[MAX_PROCESSES]; // just using this as a hack in place of kmalloc
static struct process_list *process_list = NULL;

static int next_pid = 0;
static uintptr_t current_stack = PROC_MEM_BASE;

void scheduler_init() {
    
}

// TODO: should scheduler be concerned with process creation, or just execution?
void scheduler_create_process(void (*start)(void)) {
    if (next_pid >= MAX_PROCESSES) {
        return;
    }

    current_stack += PROC_MEM_SIZE;

    process *p = &processes[next_pid];
    p->pid = (uint8_t) next_pid;
    p->status = CREATED;
    p->program_counter = (uintptr_t) start;
    p->stack_pointer = (uintptr_t) current_stack;

    current_process = p;

    next_pid++;
}
    #include "rstdlib.h"

// TODO: more sophisticated scheduling
void scheduler_switch_process() {
    if (next_pid == 0) {
        return;
    }

    uint8_t current_pid = current_process->pid;
    uint8_t next_pid = (current_pid + 1) % next_pid;

    if (next_pid == current_pid) {
        return;
    }

    if (current_process->status == RUNNING) {
        // uart_puts("SAVED\r\n");
        process_save_context(&current_process->program_counter, &current_process->stack_pointer);
        // char buffer[32];
        // hextostring(current_process->program_counter, buffer);
        // uart_puts(buffer);
        // uart_puts("\r\n");

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
    }

    process *next = &processes[next_pid];

    if (next->status == RUNNING) {
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
    }

    next->status = RUNNING;
    current_process = next;

    process_restore_context(next->program_counter, next->stack_pointer);
}