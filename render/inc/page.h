#ifndef PAGE_H
#define PAGE_H

#include <stddef.h>

typedef struct s_page t_page;

struct s_page {
    size_t  total_space;
    size_t  free_space;
    size_t  block_count;
    t_page  *next;
};

void*   request_memory_from_page(size_t size);

#endif // PAGE_H