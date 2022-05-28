#include "process.h"

static process current_process;

trap_frame* process_get_trap_frame() {
    return current_process.trapframe;
}

void process_set_trap_frame(uintptr_t sp) {
    current_process.trapframe = (trap_frame*) sp;
}

void process_set_stack(uintptr_t sp) {
    current_process.stack = (uint8_t*) sp;
}
