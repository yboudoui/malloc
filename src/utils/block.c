#include "utils.h"

t_block* fragment_block(t_block* block, size_t size)
{
    t_block  *new_block;
    size_t   rest_size;
    size_t   total_size;
    t_block  *next_phys;

    remove_block_from_bins(block); 
    total_size = get_block_size(block);
    
    if (total_size >= size + SIZEOF_BLOCK + ALIGNMENT) {
        rest_size = total_size - (size + SIZEOF_BLOCK);
        block->size = size;
        
        new_block = (t_block*)addr_offset(block, SIZEOF_BLOCK + size);
        new_block->size = rest_size;
        new_block->page = block->page;
        new_block->prev_block = block;
        new_block->next = NULL;
        new_block->prev = NULL;

        next_phys = get_next_block(new_block);
        if (next_phys) next_phys->prev_block = new_block;

        insert_block_to_bins(new_block);
    }

    set_block_not_free(block);
    block->page->block_count += 1;
    
    return (block);
}

t_block* coalesce_block(t_block* block)
{
    t_block* next;
    // t_block* prev;

    if (block == NULL) return (NULL);

    block->size = get_block_size(block);

    next = get_next_block(block);
    while (next != NULL && (next->size & FREE)) {
        remove_block_from_bins(next);
        block->size += SIZEOF_BLOCK + get_block_size(next);
        block->page->block_count -= 1;
        next = get_next_block(block);
    }
    if (next) next->prev_block = block;


    #if 0
    // Merge with Prev if free
    prev = block->prev_block;
    if (prev != NULL && (prev->size & FREE)) {
        remove_block_from_bins(prev); // Remove neighbor from list
        new_size = UNFLAG(prev->size) + SIZEOF_BLOCK + UNFLAG(block->size);
        prev->size = new_size | FREE;
        
        t_block *next_of_block = get_next_block(block);
        if (next_of_block) next_of_block->prev_block = prev;
        
        block = prev; // We are now the previous block
    }
    #endif

    return (block);
}