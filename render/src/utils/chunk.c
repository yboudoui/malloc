#include "chunk.h"

void*   get_addr_from_chunk(t_chunk *chunk)
{
    if (chunk == NULL)
        return (NULL);
    return ((char*)chunk + sizeof(t_chunk));
}

t_chunk*   get_chunk_from_addr(void *addr)
{
    if (addr == NULL)
        return (NULL);
    return ((char*)addr - sizeof(t_chunk));
}