#ifndef SYSCALL_H
#define SYSCALL_H

#include <stdint.h>

#define SYSCALL_UART 0

long syscall(uint64_t);

#endif