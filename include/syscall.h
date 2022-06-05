#ifndef SYSCALL_H
#define SYSCALL_H

#include <stdint.h>

#define SYSCALL_UART_OUT 8
#define SYSCALL_EXIT 60

uint64_t syscall(uint64_t, ...);

#endif
