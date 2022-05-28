#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <stdint.h>

#define EXCEPTION_SVC 0b010101

void set_exception_link(uint64_t);

void return_from_exception(void);

uint8_t get_exception_class(void);

uint16_t get_exception_svc_value(void);

#endif