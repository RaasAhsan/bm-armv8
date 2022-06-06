#include <stdlib.h>

#include "scheduler.h"
#include "uart.h"
#include "exception.h"
#include "process.h"
#include "kmalloc.h"

// TODO: this probably belongs elsewhere
#define NUM_MAX_PROC 128
#define PROC_MEM_BASE 0x40200000
#define PROC_MEM_SIZE 0x2000

// implements roundrobin scheduling
static struct process_list runqueue;
static struct process *current = NULL;

static int pid_count = 0;
static uintptr_t current_stack = PROC_MEM_BASE;

void scheduler_init() {
    process_list_init(&runqueue);
}

void scheduler_create_process(void (*start)(void)) {
    if (pid_count >= NUM_MAX_PROC) {
        return;
    }

    current_stack += PROC_MEM_SIZE;

    process *p = (process*) kmalloc(sizeof(process));
    p->pid = (uint8_t) pid_count;
    p->status = CREATED;
    p->program_counter = (uintptr_t) start;
    p->stack_pointer = (uintptr_t) current_stack;

    process_list_push(&runqueue, p);

    pid_count++;
}

// TODO: more sophisticated scheduling
void scheduler_switch_process() {
    if (pid_count == 0) {
        return;
    }

    struct process *next = process_list_pop(&runqueue);
    if (next == NULL) {
        next = current;
    }

    if (current != NULL && next->pid == current->pid) {
        return;
    }

    if (current != NULL && current->status == RUNNING) {
        process_save_context(&current->program_counter, &current->stack_pointer);
        
        current->context.x0 = trapframe->x0;
        current->context.x1 = trapframe->x1;
        current->context.x2 = trapframe->x2;
        current->context.x3 = trapframe->x3;
        current->context.x4 = trapframe->x4;
        current->context.x5 = trapframe->x5;
        current->context.x6 = trapframe->x6;
        current->context.x7 = trapframe->x7;
        current->context.x8 = trapframe->x8;
        current->context.x30 = trapframe->x30;
    }

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

    if (current != NULL) {
        process_list_push(&runqueue, current);
    }
    current = next;

    process_restore_context(next->program_counter, next->stack_pointer);
}

void scheduler_exit_current_process(void) {

}