#include "scheduler.h"
#include "kernel.h"
#include "uart.h"
#include "exception.h"

#define MAX_PROCESSES 128

static process processes[MAX_PROCESSES];

static int process_id = 0;
static int process_count = 0;
static uint8_t active_process_idx = -1;

void scheduler_create_process(void (*start)(void)) {
    if (process_count >= MAX_PROCESSES) {
        return;
    }

    process_count++;

    process p;
    p.pid = (uint8_t) process_id;
    p.status = CREATED;
    p.pc = (uintptr_t) start;

    processes[process_id] = p;
    process_id++;
}

// TODO: more sophisticated scheduling
void scheduler_switch_process() {
    if (process_count == 0) {
        return;
    }

    if (active_process_idx == -1) {
        active_process_idx = 0;
        scheduler_resume_process(&processes[active_process_idx]);
        return;
    }

    process *current = &processes[active_process_idx];
    // current->pc = get_elr_el1();
    // current->lr = get_lr();
    current->status = PAUSED;

    active_process_idx += 1;
    active_process_idx = active_process_idx % process_count;
    
    scheduler_resume_process(&processes[active_process_idx]);
}

void scheduler_resume_process(process *p) {
    // set_elr_el1(p->pc);

    p->status = RUNNING;

    // drop_privilege();

    __asm ("mov x30, %[lr]" : : [lr] "r" (p->lr));

    uart_putchar(0x30 + p->pid);
    return_from_exception();
}
