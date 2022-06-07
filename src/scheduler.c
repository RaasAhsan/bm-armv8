#include <stddef.h>

#include "scheduler.h"
#include "uart.h"
#include "exception.h"
#include "process.h"
#include "kmalloc.h"

// implements roundrobin scheduling
static struct process_list runqueue;
static struct process *current = NULL;

void scheduler_init() {
    process_list_init(&runqueue);
}

void scheduler_schedule_process(process *p) {
    process_list_push(&runqueue, p);
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
    if (current == NULL && runqueue.head == NULL) {
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
    // TODO: does this live elsewhere?
    current = NULL;

    struct process *next = process_list_pop(&runqueue);
    if (next == NULL) {
        // TODO: go to idle process
        return;
    }

    scheduler_resume_process(next);
}