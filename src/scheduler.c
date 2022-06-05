#include <stdlib.h>

#include "scheduler.h"
#include "uart.h"
#include "exception.h"
#include "process.h"
#include "kmalloc.h"

// TODO: this probably belongs elsewhere
#define MAX_PROCESSES 128
#define PROC_MEM_BASE 0x40200000
#define PROC_MEM_SIZE 0x2000

// implements roundrobin scheduling

struct runqueue {
    struct process_list *head;
    struct process_list *tail;
} runqueue;

// Process list
static struct process_list *current;

static int pid_count = 0;
static uintptr_t current_stack = PROC_MEM_BASE;

static void runqueue_insert(process *p) {
    struct process_list *next = (struct process_list*) kmalloc(sizeof(struct process_list));
    next->p = p;
    next->next = NULL;
    if (runqueue.tail == NULL) {
        runqueue.head = next;
        runqueue.tail = next;
        current = next;
    } else {
        runqueue.tail->next = next;
        runqueue.tail = runqueue.tail->next;
    }
}

void scheduler_init() {
    runqueue.head = NULL;
    runqueue.tail = NULL;
}

void scheduler_create_process(void (*start)(void)) {
    if (pid_count >= MAX_PROCESSES) {
        return;
    }

    current_stack += PROC_MEM_SIZE;

    process *p = (process*) kmalloc(sizeof(process));
    p->pid = (uint8_t) pid_count;
    p->status = CREATED;
    p->program_counter = (uintptr_t) start;
    p->stack_pointer = (uintptr_t) current_stack;

    runqueue_insert(p);

    pid_count++;
}

// TODO: more sophisticated scheduling
void scheduler_switch_process() {
    if (pid_count == 0) {
        return;
    }

    struct process_list *next = NULL;
    if (current->next != NULL) {
        next = current->next;
    } else {
        next = runqueue.head;
    }

    process *current_proc = current->p;
    process *next_proc = next->p;

    if (next_proc->pid == current_proc->pid) {
        return;
    }

    if (current_proc->status == RUNNING) {
        process_save_context(&current_proc->program_counter, &current_proc->stack_pointer);
        
        current_proc->context.x0 = trapframe->x0;
        current_proc->context.x1 = trapframe->x1;
        current_proc->context.x2 = trapframe->x2;
        current_proc->context.x3 = trapframe->x3;
        current_proc->context.x4 = trapframe->x4;
        current_proc->context.x5 = trapframe->x5;
        current_proc->context.x6 = trapframe->x6;
        current_proc->context.x7 = trapframe->x7;
        current_proc->context.x8 = trapframe->x8;
        current_proc->context.x30 = trapframe->x30;
    }

    if (next_proc->status == RUNNING) {
        // Set trapframe to current trapframe
        trapframe->x0 = next_proc->context.x0;
        trapframe->x1 = next_proc->context.x1;
        trapframe->x2 = next_proc->context.x2;
        trapframe->x3 = next_proc->context.x3;
        trapframe->x4 = next_proc->context.x4;
        trapframe->x5 = next_proc->context.x5;
        trapframe->x6 = next_proc->context.x6;
        trapframe->x7 = next_proc->context.x7;
        trapframe->x8 = next_proc->context.x8;
        trapframe->x30 = next_proc->context.x30;
    }

    next_proc->status = RUNNING;
    current = next;

    process_restore_context(next_proc->program_counter, next_proc->stack_pointer);
}