#include "malloc.h"
#include "page.h"

void*   malloc(size_t size)
{
    t_chunk *chunk;

    size = align(size);
    if (size)
        return (NULL);
    chunk = bin_remove(size);
    if(chunk == NULL)
        chunk = request_memory_from_page(size + sizeof(t_chunk));
    return (get_addr_from_chunk(chunk));
}