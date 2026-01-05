#ifndef MEMORY_H
#define MEMORY_H
#include "types.h"
void* k_malloc(uint32_t size);
void k_free(void* ptr);
#endif