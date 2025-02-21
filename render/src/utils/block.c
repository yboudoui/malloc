#include "utils.h"

void     init_block(t_block block, size_t size, size_t page_offset)
{
    block->prev_block_size = 0;
    block->page_offset = page_offset;
    block->next = NULL;
    block->prev = NULL;
    set_block_size(block, size);
}

t_block coalesce(t_block block)
{
    t_page  page;
    t_block next;
    t_block prev;
    size_t  size;
    size_t page_offset;

    page_offset = block->page_offset;
    size = UNFLAG(block->size);
    remove_block(block);
    page = get_page_from_block(block);
    
    next = get_next_block(block);
    while (next && (next->size & FREE)) {
        remove_block(next);
        size += SIZEOF_BLOCK + UNFLAG(next->size);
        page->block_count -= 1;
        next = get_next_block(next);
    }

    prev = get_prev_block(block);
    while (prev && (prev->size & FREE)) {
        page_offset = prev->page_offset;
        remove_block(prev);
        size += SIZEOF_BLOCK + UNFLAG(prev->size);
        page->block_count -= 1;
        prev = get_prev_block(prev);
    }
    init_block(block, size, page_offset);
    return (block);
    // return (set_block_size(block, size));
}

/*
733
*/