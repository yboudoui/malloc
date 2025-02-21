#include "utils.h"

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

t_block set_block_flag(t_block block, size_t flag)
{
    t_block next_block;

    next_block = addr_offset(block, SIZEOF_BLOCK + UNFLAG(block->size));
    block->size |= flag;
    next_block->prev_block_size = block->size;
    return (block);
}

void unset_block_flag(t_block block, size_t flag)
{
    t_block next_block;

    next_block = addr_offset(block, SIZEOF_BLOCK + UNFLAG(block->size));
    block->size &= ~flag;
    next_block->prev_block_size = block->size;
}

t_block set_block_size(t_block block, size_t size)
{
    t_block next_block;

    next_block = addr_offset(block, SIZEOF_BLOCK + size);
    block->size = size;
    next_block->prev_block_size = UNFLAG(block->size);
    return (block);
}

