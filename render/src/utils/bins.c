#include "bins.h"

t_block bins[MAX_BINS] = {NULL};

static int get_bin_index(size_t size)
{
    if (size > MAX_FAST_SIZE)
        size -= MAX_FAST_SIZE;
    return (size / ALIGNMENT);
}

void    bin_append(t_block block)
{
    size_t  index;
    
    index = get_bin_index(block->curr_block_size);
    block->owner = NULL;
    block->next = bins[index];
    bins[index]->owner = &block;
    bins[index] = block;
}

t_block bin_remove(size_t size)
{
    size_t  index;
    t_block block;

    index = get_bin_index(size);
    if (bins[index] == NULL)
        return (NULL);
    block = bins[index];
    bins[index] = block->next;
    bins[index]->owner = NULL;
    return (block);
}

