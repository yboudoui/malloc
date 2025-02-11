#ifndef MALLOC_H
#define MALLOC_H

#include "bins.h"

// Utils
void    coalesce(t_block* chunk);

// Exported functions
#define EXPORT __attribute__((visibility("default")))

EXPORT void free(void *ptr);
EXPORT void *malloc(size_t size);
EXPORT void *realloc(void *ptr, size_t size);

#endif //MALLOC_H