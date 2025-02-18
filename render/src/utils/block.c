#include "utils.h"

size_t  get_unflaged_size(t_size size)
{
    return (size.raw - size.flag.flag);
}

void*   get_addr_from_block(t_block block)
{
    unsigned char    *ptr;

    ptr = (unsigned char*)block;
    if (ptr == NULL) return (NULL);
    ptr += SIZEOF_BLOCK;
    return ((void*)ptr);
}

t_block get_block_from_addr(void *addr)
{
    unsigned char*   ptr;

    ptr = (unsigned char*)addr;
    if (ptr == NULL) return (NULL);
    ptr -= SIZEOF_BLOCK;
    return ((t_block)ptr);
}

t_size*  get_tail_metadata(t_block block)
{
    char*   addr;

    addr = (char*)block;
    addr += SIZEOF_BLOCK;
    addr += get_unflaged_size(block->curr_block_size);
    return ((t_size*)addr);
}

t_block    set_block_to_free(t_block block)
{
    t_size  *tail_metadata;

    block->curr_block_size.flag.flag |= 1;
    tail_metadata = get_tail_metadata(block);
    tail_metadata->flag.flag |= 1;
    return (block);
}

t_block    set_block_to_not_free(t_block block)
{
    t_size  *tail_metadata;

    block->curr_block_size.flag.flag |= ~1;
    tail_metadata = get_tail_metadata(block);
    // tail_metadata->flag.flag |= ~1;

    tail_metadata->raw = block->curr_block_size.raw;
    return (block);
}

