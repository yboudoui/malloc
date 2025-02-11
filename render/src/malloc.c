#include "malloc.h"
#include "page.h"

static inline size_t align(size_t size)
{
    return (size + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1);
}

void*   malloc(size_t size)
{
    t_block *chunk;

    if (size == 0) return (NULL);
    size = align(size);
    chunk = bin_remove(size);
    if(chunk == NULL)
        chunk = request_memory_from_page(size + sizeof(t_block));
    return (get_addr_from_chunk(chunk));
}