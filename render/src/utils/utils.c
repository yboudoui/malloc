#include "utils.h"

void    *ft_bzero(void* addr, size_t size)
{
    while (addr && size) ((uint8_t*)addr)[--size] = 0;
    return (addr);
}

t_page   get_page_from_block(t_block block)
{
    uint8_t *addr;

    if (block == NULL) return (NULL);
    addr = (uint8_t*)block;
    addr -= block->page_offset;
    return ((t_page)addr);
}

t_block  get_next_block(t_block block)
{
    size_t  size;

    if (block == NULL) return (NULL);
    size = UNFLAG(block->size);
    if (size == 0) return (NULL);
    size += SIZEOF_BLOCK;
    return (addr_offset(block, +size));
}

t_block  get_prev_block(t_block block)
{
    size_t  size;

    if (block == NULL) return (NULL);
    size = UNFLAG(block->prev_block_size);
    if (size == 0) return (NULL);
    size += SIZEOF_BLOCK;
    return (addr_offset(block, -size));
}

void*   addr_offset(void *addr, size_t offset) {
    return (addr ? (uint8_t*)addr + offset : NULL);
}