#include "utils.h"

#include <unistd.h>
#include <assert.h>

void alloc_block(t_block *block)
{
    block->next = NULL;
    block->prev = NULL;
    insert_block_to_bins(block);
    set_block_free(block);
    block->page->block_count += 1;
}

void free_block(t_block *block)
{
    remove_block_from_bins(block);
    set_block_not_free(block);
    block->page->block_count -= 1;
}

t_block* fragment_block(t_block* block, size_t size)
{
    t_block  *new_block;
    size_t   rest_size;
    size_t   total_size;
    t_block  *next_phys;

    free_block(block);

    total_size = get_block_size(block);
    rest_size = total_size - (SIZEOF_BLOCK + size);

    block->size = size;
    
    new_block = (t_block*)addr_offset(block, SIZEOF_BLOCK + size);
    new_block->size = rest_size;
    new_block->page = block->page;

    alloc_block(new_block);
    return (block);
}

t_block* coalesce_block(t_block* block)
{
    t_block* next;

    if (block == NULL) return (NULL);

    block->size = get_block_size(block);

    next = get_next_physical_block(block);
    while (next != NULL && (next->size & FREE)) {
        free_block(next);

        block->size += SIZEOF_BLOCK + get_block_size(next);

        next = get_next_physical_block(block);
    }
    return (block);
}