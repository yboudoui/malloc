#include "utils.h"

void*   addr_offset(void *addr, size_t offset)
{
    return (addr ? (uint8_t*)addr + offset : NULL);
}

void    *ft_bzero(void* addr, size_t size)
{
    while (addr && size) ((uint8_t*)addr)[--size] = 0;
    return (addr);
}

t_block*  get_next_block(t_block* block)
{
    size_t  size;

    if (block == NULL) return (NULL);
    size = UNFLAG(block->size);
    if (size == 0) return (NULL);
    size += SIZEOF_BLOCK;
    return (addr_offset(block, +size));
}