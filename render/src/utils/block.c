#include "utils.h"
#include <unistd.h>

t_block* fragment_block(t_block* block, size_t size)
{
    t_block* new_block;
    size_t   rest_size;
    size_t   total_size;

    // Assumes 'block' is currently in bins. Remove it to modify.
    remove_block_from_bins(block); 

    total_size = UNFLAG(block->size);
    
    if (total_size >= size + SIZEOF_BLOCK + ALIGNMENT) {
        rest_size = total_size - (size + SIZEOF_BLOCK);
        
        block->size = size | (block->size & FREE);
        
        new_block = (t_block*)addr_offset(block, SIZEOF_BLOCK + size);
        new_block->size = rest_size | FREE;
        new_block->page = block->page;
        new_block->prev_block = block;
        new_block->next = NULL;
        new_block->prev = NULL;
        // new_block->padding = 0; // implicit by bzero/malloc logic usually

        #if 1
                // 3. Link the next physical block... BUT CAREFULLY!
        // We need to know where the page ends to avoid Segfaulting.
        t_block *next_phys = get_next_block(new_block);
        
        // Calculate the limit of the page
        size_t zone_size = compute_mmap_size(block->page->type, 0);
        void *page_end = (void*)block->page + zone_size;

        // CRITICAL CHECK:
        // Only try to update the next neighbor if it is actually inside the page.
        // If next_phys == page_end, we are at the edge and must stop.
        if ((void*)next_phys < page_end) {
            next_phys->prev_block = new_block;
        }
        #else
        t_block *next_phys = get_next_block(new_block);
        if (next_phys) {
            next_phys->prev_block = new_block;
            write(1, "MY MALLOC CALLED8\n", 18); // Debug print
        }
        #endif

        insert_block_to_bins(new_block);
    }

    
    block->size &= ~FREE;
    block->page->block_count++;
    
    return (block);
}

t_block* coalesce_block(t_block* block)
{
    t_block* next;
    t_block* prev;
    size_t   new_size;

    if (block == NULL) return (NULL);
    
    // Ensure it is marked free
    block->size |= FREE;

    // Merge with Next if free
    next = get_next_block(block);
    if (next && (next->size & FREE)) {
        remove_block_from_bins(next); // Remove neighbor from list
        new_size = UNFLAG(block->size) + SIZEOF_BLOCK + UNFLAG(next->size);
        block->size = new_size | FREE;
        
        t_block *next_next = get_next_block(next);
        if (next_next) next_next->prev_block = block;
    }

    // Merge with Prev if free
    prev = block->prev_block;
    if (prev && (prev->size & FREE)) {
        remove_block_from_bins(prev); // Remove neighbor from list
        new_size = UNFLAG(prev->size) + SIZEOF_BLOCK + UNFLAG(block->size);
        prev->size = new_size | FREE;
        
        t_block *next_of_block = get_next_block(block);
        if (next_of_block) next_of_block->prev_block = prev;
        
        block = prev; // We are now the previous block
    }

    return (block);
}