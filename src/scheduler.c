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

static void scheduler_pause_process(process *p) {
    process_save_context(&p->program_counter, &p->stack_pointer);
    p->context.x0 = trapframe->x0;
    p->context.x1 = trapframe->x1;
    p->context.x2 = trapframe->x2;
    p->context.x3 = trapframe->x3;
    p->context.x4 = trapframe->x4;
    p->context.x5 = trapframe->x5;
    p->context.x6 = trapframe->x6;
    p->context.x7 = trapframe->x7;
    p->context.x8 = trapframe->x8;
    p->context.x30 = trapframe->x30;

    process_list_push(&runqueue, current);
    current = NULL;
}

static void scheduler_resume_process(process *p) {
    // Set trapframe to stored process trapframe if it had been running
    if (p->status == RUNNING) {
        trapframe->x0 = p->context.x0;
        trapframe->x1 = p->context.x1;
        trapframe->x2 = p->context.x2;
        trapframe->x3 = p->context.x3;
        trapframe->x4 = p->context.x4;
        trapframe->x5 = p->context.x5;
        trapframe->x6 = p->context.x6;
        trapframe->x7 = p->context.x7;
        trapframe->x8 = p->context.x8;
        trapframe->x30 = p->context.x30;
    }
    p->status = RUNNING;
    current = p;
    process_restore_context(p->program_counter, p->stack_pointer);
}

// TODO: more sophisticated scheduling
void scheduler_context_switch() {
    if (pid_count == 0) {
        return;
    }

    struct process *next = process_list_pop(&runqueue);
    if (next == NULL) {
        next = current;
    }

    if (current != NULL) {
        // No need to context switch if we are resuming the same process
        if (next->pid == current->pid) {
            return;
        }

        scheduler_pause_process(current);
    }

    scheduler_resume_process(next);
}

void scheduler_exit_process(void) {
    // TODO: free memory from process
    // TODO: notify parent/etc of child process termination if waiting
    current = NULL;

    struct process *next = process_list_pop(&runqueue);
    if (next == NULL) {
        // TODO: go to idle process
        return;
    }

    scheduler_resume_process(next);
}