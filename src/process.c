#include "process.h"
#include "kmalloc.h"

#include <stddef.h>

trap_frame *trapframe = NULL;

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
