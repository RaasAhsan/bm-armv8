#include <stdint.h>

#include "syscall.h"

static char table[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

void *hextostring(uint64_t input, char *buffer) {
    do  {
        int index = input & 0xf;
        *buffer = table[index];
        buffer++;
        input >>= 4;
    } while (input > 0);

    *buffer = '\0';
}

void exit() {
    syscall(SYSCALL_EXIT);
}
