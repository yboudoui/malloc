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
    addr += block->curr_block_size;
    return ((size_t*)addr);
}

static void set_tail_metadata(t_block block, size_t size)
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

void    set_block_to_free(t_block block)
{
    size_t  size;

    size = block->curr_block_size;
    size |= 1;
    set_tail_metadata(block, size);
    block->curr_block_size = size;
}