#include "block.h"

static t_block  get_next_block(t_block block)
{
    t_block next_block;

    next_block = (t_block)get_tail_metadata(block);
    if (next_block->prev_block_size & 2)
        return (next_block);
    return (NULL);
}

static t_block  get_prev_block(t_block block)
{
    if (block->prev_block_size & 2)
    {
        // TODO: be careful with the flags in size
    }
    return (NULL);
}

static t_block  is_block_free(t_block block)
{
    if (block && (block->curr_block_size & 1))
        return (block);
    return (NULL);
}

static t_block  merge_block(t_block src, t_block dest)
{
    dest->curr_block_size += SIZEOF_BLOCK + src->curr_block_size;
    set_tail_metadata(dest, dest->curr_block_size);
    remove_block(src);
    // TODO: take into consideration the number of block register in t_page
    return (dest);
}

t_block coalesce(t_block block)
{
    t_block next;
    t_block prev;

    next = is_block_free(get_next_block(block));
    while (next) {
        block = merge_block(next, block);
        next = is_block_free(get_next_block(block));
    }

    prev = is_block_free(get_prev_block(block));
    while (prev) {
        block = merge_block(prev, block);
        prev = is_block_free(get_prev_block(block));
    }

    return (block);
}