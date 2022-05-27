#include <stdint.h>

typedef enum process_status {
    CREATED,
    PAUSED,
    RUNNING,
    EXITED
} process_status;

typedef struct process {
    uint8_t pid;
    process_status status;
    uintptr_t pc;
    uintptr_t lr;
} process;

void scheduler_create_process(void (*)(void));
void scheduler_switch_process();
void scheduler_resume_process(process *p);
