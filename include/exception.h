#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <stdint.h>

#define EXCEPTION_SVC 0b010101

#define INTERRUPT_SGI 8
#define INTERRUPT_TIMER 27
#define INTERRUPT_UART 33

void register_isr(uint16_t, void (*)(void));

void set_exception_link(uint64_t);

void return_from_exception(void);

// TODO: potentially we can just load these into the trap frame
uint8_t get_exception_class(void);

uint16_t get_exception_svc_value(void);

#endif