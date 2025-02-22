#include "utils.h"
/*
t_block coalesce(t_block block)
{
    t_block prev;
    t_page  page;
    t_block next;

    prev = get_prev_block(block);
    while (prev && (prev->size & FREE)) {
        block = prev;
        prev = get_prev_block(block);
    }
    page = get_page_from_block(block);

    block->size = UNFLAG(block->size);
    next = get_next_block(block);
    while (next && (next->size & FREE)) {
        block->size += SIZEOF_BLOCK + UNFLAG(next->size);
        remove_block_from_bins(next);
        page->block_count -= 1;
        page->used_space -= SIZEOF_BLOCK + UNFLAG(next->size);
        next = get_next_block(next);
    }
    remove_block_from_bins(block);
    get_next_block(block)->prev_block_size = block->size;
    return (block);
}
*/

t_block coalesce(t_block block)
{
    t_page  page;
    t_block next;

    while (block->prev_block_size & FREE)
        block = addr_offset(block, -(SIZEOF_BLOCK + UNFLAG(block->prev_block_size)));
    page = get_page_from_block(block);

    block->size = UNFLAG(block->size);
    next = get_next_block(block);
    while (next && (next->size & FREE)) {
        block->size += SIZEOF_BLOCK + UNFLAG(next->size);
        remove_block_from_bins(next);
        page->block_count -= 1;
        page->used_space -= SIZEOF_BLOCK + UNFLAG(next->size);
        next = get_next_block(next);
    }
    remove_block_from_bins(block);
    get_next_block(block)->prev_block_size = block->size;
    return (block);
}