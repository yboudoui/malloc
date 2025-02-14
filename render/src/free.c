#include "malloc.h"

void free(void* addr)
{
    t_block* block;

    block = get_block_from_addr(addr);
    if (block == NULL)
        return;


    size_t size = block->size;

    // Mark the block as free
    block->free = 1;

    // Coalesce with adjacent free blocks
    coalesce(block);

    bin_append(size, block);
    free(block);
}