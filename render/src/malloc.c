#include "malloc.h"
#include "page.h"

static inline size_t align(size_t size)
{
    return (size + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1);
}

void*   malloc(size_t size)
{
    t_block block;

    if (size == 0) return (NULL);
    size = align(size);
    block = bin_remove(size);
    if(block == NULL)
        block = request_new_block(size);
    return (get_addr_from_block(block));
}