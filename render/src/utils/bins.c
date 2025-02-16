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

    size = get_unflaged_size(block->curr_block_size);
    index = get_bin_index(size);
    block->prev = NULL;
    block->next = bins[index];
    if (bins[index])
        bins[index]->prev = block;
    bins[index] = block;
}

t_block request_available_block(size_t size)
{
    size_t  index;
    t_block block;

    index = get_bin_index(size);
    if (bins[index] == NULL)
        return (NULL);
    block = bins[index];
    bins[index] = block->next;
    if (bins[index])
        bins[index]->prev = NULL;
    return (block);
}

