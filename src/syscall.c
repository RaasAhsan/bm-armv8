#include <stdarg.h>

#include "syscall.h"

long svc_syscall(uint64_t);

long syscall(uint64_t number) {
   
    svc_syscall(number);
}
