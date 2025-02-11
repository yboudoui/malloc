#include "malloc.h"

static t_block  get_next_block(t_block block)
{
    size_t  *tail_metadata;

    tail_metadata = get_tail_metadata(block);
    if ((*tail_metadata) & 2)
        return ((char*)tail_metadata + sizeof(size_t));
    return (NULL);
}

static t_block  get_next_free_block(t_block block)
{
    t_block next_block;

    next_block = get_next_block(block);
    if (next_block && (next_block->curr_block_size & 1))
        return (next_block);
    return (NULL);
}

static t_block  forward_coalesce(t_block block)
{
    t_block next_free_block;

    next_free_block = get_next_free_block(block);
    while (next_free_block)
    {
        block->curr_block_size += sizeof(size_t) + SIZEOF_BLOCK;
        block->curr_block_size += next_free_block->curr_block_size;
        set_tail_metadata(block, block->curr_block_size);
        next_free_block = get_next_free_block(block);
    }
    return (block);
}

void coalesce(t_block block)
{
    block = forward_coalesce(block);

}
