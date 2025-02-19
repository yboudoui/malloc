#ifndef MALLOC_H
#define MALLOC_H

#include <stddef.h>

#define EXPORT __attribute__((visibility("default")))

EXPORT void free(void *ptr);
EXPORT void *malloc(size_t size);
// EXPORT void *realloc(void *ptr, size_t size);

EXPORT void show_alloc_mem(void);
EXPORT void print_fd(int fd, const char *format, ...);

#endif //MALLOC_H