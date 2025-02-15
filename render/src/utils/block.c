#include "block.h"

void*   get_addr_from_block(t_block block)
{
    if (block == NULL)
        return (NULL);
    return ((void*)(char*)block + SIZEOF_BLOCK);
}

t_block get_block_from_addr(void *addr)
{

    if (addr == NULL)
        return (NULL);
    return ((t_block)(char*)addr - SIZEOF_BLOCK);
}

size_t*  get_tail_metadata(t_block block)
{
    char*   addr;

    addr = (char*)block;
    addr += SIZEOF_BLOCK;
    // be careful with the flags in block->curr_block_size
    addr += block->curr_block_size;
    return ((size_t*)addr);
}

void set_tail_metadata(t_block block, size_t size)
{
    size_t  *tail_metadata;

    tail_metadata = get_tail_metadata(block);
    (*tail_metadata) = size;
}

t_block set_block(t_block block, size_t size, size_t page_offset)
{
    block->prev_block_size |= 2;
    block->page_offset = page_offset;
    block->curr_block_size = size;
    set_tail_metadata(block, size);
    return (block);
}

t_block    set_block_to_free(t_block block)
{
    size_t  size;

    size = block->curr_block_size;
    size |= 1;
    set_tail_metadata(block, size);
    block->curr_block_size = size;
    return (block);
}

void    remove_block(t_block block)
{
    if (block->prev)
        block->prev->next = block->next;
    if (block->next)
        block->next->prev = block->prev;
}

/*
inline size_t toggle_free(size_t number)
{
    return number ^ ((size_t)1 << 0);
}

inline size_t is_free(size_t number)
{
    return (number >> 0) & (size_t)1;
}
*/
