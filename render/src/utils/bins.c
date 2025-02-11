#include "bins.h"

t_block* bins[MAX_BINS] = {NULL};

static int get_bin_index(size_t size)
{
    if (size > MAX_FAST_SIZE)
        size -= MAX_FAST_SIZE;
    return (size / ALIGNMENT);
}

void    bin_append(size_t size, t_block *chunk)
{
    size_t  index;
    
    index = get_bin_index(size);
    chunk->next = bins[index];
    bins[index] = chunk;
}

t_block*    bin_remove(size_t size)
{
    size_t      index;
    t_block     *chunk;

    index = get_bin_index(size);
    if (bins[index] == NULL)
        return (NULL);
    chunk = bins[index];
    bins[index] = chunk->next;
    return (chunk);
}

