#include <stdint.h>
#include "process.h"

void scheduler_init(void);
void scheduler_create_process(void (*)(void));
void scheduler_switch_process(void);
void scheduler_resume_process(process *p);
void scheduler_exit_current_process(void);
