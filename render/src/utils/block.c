#include "utils.h"

size_t  get_unflaged_size(size_t size)
{
    return (size & ~0b11);
}

void*   get_addr_from_block(t_block block)
{
    char    *ptr;
    if (block == NULL) return (NULL);
    ptr = (char*)block;
    ptr += SIZEOF_BLOCK;
    return ((void*)ptr);
}

t_block get_block_from_addr(void *addr)
{
    char*   ptr;

    ptr = addr;
    if (ptr == NULL) return (NULL);
    ptr -= SIZEOF_BLOCK;
    return ((t_block)ptr);
}

size_t*  get_tail_metadata(t_block block)
{
    char*   addr;

    addr = (char*)block;
    addr += SIZEOF_BLOCK;
    addr += get_unflaged_size(block->curr_block_size);
    return ((size_t*)addr);
}

void set_tail_metadata(t_block block, size_t size)
{
    size_t  *tail_metadata;

    tail_metadata = get_tail_metadata(block);
    (*tail_metadata) = size;
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


