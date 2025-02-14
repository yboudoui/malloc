#include "block.h"

void*   get_addr_from_block(t_block *block)
{
    if (block == NULL)
        return (NULL);
    return ((char*)block + sizeof(t_block));
}

t_block*   get_block_from_addr(void *addr)
{
    char    *ptr;

    if (addr == NULL)
        return (NULL);
    ptr = (char*)addr;
    ptr -= sizeof(t_block);
    return ((t_block*)ptr);
}