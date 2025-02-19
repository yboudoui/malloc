#include "utils.h"

void    remove_block(t_block block)
{
    if (block->next)
        block->next->prev = block->prev;
    if (block->prev)
        block->prev->next = block->next;
}

static t_block  merge_block(t_block src, t_block dst)
{
    remove_block(src);
    set_block_size(dst, UNFLAG(dst->size) + SIZEOF_BLOCK + UNFLAG(src->size));
    get_page_from_block(dst)->block_count -= 1;
    return (dst);
}

t_block coalesce(t_block block)
{
    t_block next;
    t_block prev;

    next = get_next_block(block);
    while (next && (next->size & FREE)) {
        block = merge_block(next, block);
        next = get_next_block(block);
    }

    prev = get_prev_block(block);
    while (prev && (prev->size & FREE)) {
        block = merge_block(block, prev);
        prev = get_prev_block(block);
    }

    return (block);
}
