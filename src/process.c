#include "process.h"
#include <stdlib.h>

trap_frame *trapframe = NULL;

trap_frame* process_get_trap_frame() {
    return trapframe;
}

void process_set_trap_frame(uintptr_t sp) {
    trapframe = (trap_frame*) sp;
}
