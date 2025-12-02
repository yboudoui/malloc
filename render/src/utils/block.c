#include "utils.h"

t_block* fragment_block(t_block* block, size_t size)
{
    t_block* new_block;
    size_t   rest_size;
    size_t   total_size;

    total_size = UNFLAG(block->size);
    // Calculate remaining space. We need enough for header + min alignment
    if (total_size >= size + SIZEOF_BLOCK + ALIGNMENT) {
        rest_size = total_size - (size + SIZEOF_BLOCK);
        
        // Shrink current block
        block->size = size | (block->size & FREE); // Preserve flag (though usually alloc'd here)
        
        // Create new free block
        new_block = (t_block*)addr_offset(block, SIZEOF_BLOCK + size);
        new_block->size = rest_size | FREE;
        new_block->page = block->page;
        new_block->prev_block = block;
        new_block->next = NULL;
        new_block->prev = NULL;
        
        // Link next physical block to this new one
        t_block *next_phys = get_next_block(new_block);
        if (next_phys) next_phys->prev_block = new_block;

        // Insert remainder into bins
        insert_block_to_bins(new_block);
    }
    return (block);
}

t_block* coalesce_block(t_block* block)
{
    t_block* next;
    t_block* prev;
    size_t   new_size;

    if (block == NULL) return (NULL);
    
    block->size |= FREE;

    // Merge with Next if free
    next = get_next_block(block);
    if (next && (next->size & FREE)) {
        remove_block_from_bins(next);
        new_size = UNFLAG(block->size) + SIZEOF_BLOCK + UNFLAG(next->size);
        block->size = new_size | FREE;
        
        // Update physical link
        t_block *next_next = get_next_block(next);
        if (next_next) next_next->prev_block = block;
    }

    // Merge with Prev if free
    prev = block->prev_block;
    if (prev && (prev->size & FREE)) {
        remove_block_from_bins(prev);
        new_size = UNFLAG(prev->size) + SIZEOF_BLOCK + UNFLAG(block->size);
        prev->size = new_size | FREE;
        
        // Update physical link
        t_block *next_of_block = get_next_block(block);
        if (next_of_block) next_of_block->prev_block = prev;
        
        block = prev;
    }

    return (block);
}