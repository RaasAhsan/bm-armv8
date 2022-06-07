#include "process.h"
#include "kmalloc.h"

#include <stddef.h>

#define NUM_MAX_PROC 128
#define PROC_MEM_BASE 0x40200000
#define PROC_MEM_SIZE 0x2000

trap_frame *trapframe = NULL;

static int pid_count = 0;
static uintptr_t current_stack = PROC_MEM_BASE;

process *process_create(void (*start)(void)) {
    if (pid_count >= NUM_MAX_PROC) {
        return NULL;
    }

    current_stack += PROC_MEM_SIZE;

    process *p = (process*) kmalloc(sizeof(process));
    p->pid = (uint8_t) pid_count;
    p->status = CREATED;
    p->program_counter = (uintptr_t) start;
    p->stack_pointer = (uintptr_t) current_stack;

    pid_count++;

    return p;
}

trap_frame* process_get_trap_frame() {
    return trapframe;
}

void process_set_trap_frame(uintptr_t sp) {
    trapframe = (trap_frame*) sp;
}

void process_list_init(struct process_list *plist) {
    plist->head = NULL;
    plist->tail = NULL;
}

void process_list_push(struct process_list *plist, process *p) {
    struct process_head *node = (struct process_head*) kmalloc(sizeof(struct process_head));
    node->p = p;
    node->next = NULL;
    if (plist->tail == NULL) {
        plist->head = node;
        plist->tail = node;
    } else {
        plist->tail->next = node;
        plist->tail = node;
    }
}

process* process_list_pop(struct process_list *plist) {
    struct process_head *head = plist->head;
    if (head == NULL) {
        return NULL;
    } else {
        plist->head = head->next;
        if (head->next == NULL) {
            plist->tail = NULL;
        }
        return head->p;
    }
}
