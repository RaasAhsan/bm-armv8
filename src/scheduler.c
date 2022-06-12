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

void scheduler_schedule_process(struct process *p) {
    process_list_push(&runqueue, p);
}

static void scheduler_pause_process() {
    process_suspend(current);
    process_list_push(&runqueue, current);
    current = NULL;
}

static void scheduler_resume_process(struct process *p) {
    process_resume(p);
    current = p;
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

        scheduler_pause_process();
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