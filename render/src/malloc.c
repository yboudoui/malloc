#include "malloc.h"
#include "page.h"
#include "bins.h"

static inline size_t align(size_t size)
{
    return (size + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1);
}

void*   malloc(size_t size)
{
    t_block block;

    if (size == 0)
        return (NULL);
    size = align(size);
    block = request_available_block(size);
    if(block == NULL)
        block = request_new_block(size);
    return (get_addr_from_block(block));
}

void    free(void* addr)
{
    t_block block;

    block = get_block_from_addr(addr);
    if (block == NULL)
        return;
    block = coalesce(block);
    block = set_block_to_free(block);
    release_block(block);
    // TODO: In order to `munmap` need to take into consideration the number of block in page
}

// TODO: remove string.h in favore of a custom memcpy
#include <string.h>
void    *realloc(void *ptr, size_t size)
{
    t_block block;
    void    *new_addr;

    block = get_block_from_addr(ptr);
    if (block == NULL)
        return (NULL);
    new_addr = malloc(size);
    if (new_addr == NULL)
        return (NULL);
    // TODO: be careful with the flag in block->curr_block_size
    return (memcpy(new_addr, ptr, block->curr_block_size));
}