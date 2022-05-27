#ifndef KERNEL_H
#define KERNEL_H

#include <stdint.h>

void drop_privilege(void);

uintptr_t get_elr_el1(void);
void set_elr_el1(uintptr_t);

uintptr_t get_sp_el0(void);
void set_sp_el0(uintptr_t);

uintptr_t get_lr(void);
void set_lr(uintptr_t);

void exception_return(void);

void mask_interrupts(void);
void unmask_interrupts(void);

#endif