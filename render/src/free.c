#include "malloc.h"

void free(void* addr)
{
    t_block block;

    block = get_block_from_addr(addr);
    if (block == NULL)
        return;
    set_block_to_free(block);

    coalesce(block);

    bin_append(block);
    // munmap()
    free(block);
}