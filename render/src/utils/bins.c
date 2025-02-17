#include "utils.h"

t_block bins[MAX_BINS] = {NULL};

static int get_bin_index(size_t size)
{
    if (size > MAX_FAST_SIZE)
        size -= MAX_FAST_SIZE;
    return (size / ALIGNMENT);
}

void    release_block(t_block block)
{
    size_t  index;
    size_t  size;
    t_page  page;

    size = get_unflaged_size(block->curr_block_size);
    index = get_bin_index(size);
    if (!block->prev && !block->next && !bins[index])
        return;
    block->prev = NULL;
    block->next = bins[index];
    if (block->next)
        block->next->prev = block;
    bins[index] = block;

    page = get_page_from_block(block);
    page->block_count -= 1;
}

t_block request_available_block(size_t size)
{
    size_t  index;
    t_block block;
    t_page  page;

    index = get_bin_index(size);
    block = bins[index];
    if (block == NULL) return (NULL);
    bins[index]->prev = NULL;
    bins[index] = block->next;

    page = get_page_from_block(block);
    page->block_count -= 1;
    return (block);
}

