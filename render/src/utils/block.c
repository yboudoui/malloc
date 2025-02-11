#include "block.h"

void*   get_addr_from_chunk(t_block *chunk)
{
    if (chunk == NULL)
        return (NULL);
    return ((char*)chunk + sizeof(t_block));
}

t_block*   get_chunk_from_addr(void *addr)
{
    char    *ptr;

    if (addr == NULL)
        return (NULL);
    ptr = (char*)addr;
    ptr -= sizeof(t_block);
    return ((t_block*)ptr);
}