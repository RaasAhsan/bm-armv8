#ifndef KMALLOC_H
#define KMALLOC_H

#include <stdint.h>

void kmalloc_init(uintptr_t);
void *kmalloc(size_t);

#endif
