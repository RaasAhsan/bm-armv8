#include <stdint.h>
#include "process.h"

void scheduler_init(void);
void scheduler_schedule_process(struct process*);
void scheduler_context_switch(void);
void scheduler_exit_process(void);
