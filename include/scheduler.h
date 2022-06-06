#include <stdint.h>
#include "process.h"

void scheduler_init(void);
void scheduler_create_process(void (*)(void));
void scheduler_context_switch(void);
void scheduler_exit_process(void);
