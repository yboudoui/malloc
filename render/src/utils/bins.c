#include "utils.h"

t_block bins[MAX_BINS] = {NULL};

static size_t get_bin_index(size_t size)
{
    if (size > MAX_FAST_SIZE)
        size -= MAX_FAST_SIZE;
    return (size / ALIGNMENT);
}

void    release_block(t_block block)
{
    size_t  index;
    size_t  size;

    size = UNFLAG(block->size);
    index = get_bin_index(size);
    if (index > 126) index = 127;
    block->prev = NULL;
    block->next = bins[index];
    if (block->next)
        block->next->prev = block;
    bins[index] = block;

    get_page_from_block(block)->block_count -= 1;
    set_block_flag(block, FREE);
}

t_block request_available_block(size_t size)
{
    size_t  index;
    t_block block;

    index = get_bin_index(size);
    if (index > 126) index = 127;
    block = bins[index];
    if (block == NULL) return (NULL);
    bins[index]->prev = NULL;
    bins[index] = block->next;

    get_page_from_block(block)->block_count += 1;
    return (unset_block_flag(block, FREE));
}

