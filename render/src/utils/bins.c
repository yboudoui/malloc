#include "utils.h"

static t_block** get_bin_entry(size_t size)
{
    // Simple direct mapping for small sizes, clamped for large
    size_t idx = size / ALIGNMENT;
    if (idx >= MAX_BINS) idx = MAX_BINS - 1;
    return (&get_heap()->bins[idx]);
}

t_block* remove_block_from_bins(t_block* block)
{
    if (!block) return(NULL);
    
    if (block->next) block->next->prev = block->prev;
    if (block->prev) block->prev->next = block->next;
    
    // Update head if this was the head
    t_block **head = get_bin_entry(UNFLAG(block->size));
    if (*head == block) {
        *head = block->next;
    }

    block->next = NULL;
    block->prev = NULL;
    return (block);
}

void    insert_block_to_bins(t_block* block)
{
    if (!block) return;
    
    t_block **head = get_bin_entry(UNFLAG(block->size));
    
    block->prev = NULL;
    block->next = *head;
    
    if (*head) (*head)->prev = block;
    *head = block;
}

static t_zone_type get_size_type(size_t size)
{
    if (size <= TINY_MAX) return TINY;
    if (size <= SMALL_MAX) return SMALL;
    return LARGE;
}

t_block* request_block(size_t size)
{
    t_block* bin;
    t_page*  page;
    t_zone_type type;
    int      bin_idx;

    type = get_size_type(size);

    // 1. If not LARGE, try to find in bins
    if (type != LARGE) {
        bin_idx = size / ALIGNMENT;
        while (bin_idx < MAX_BINS) {
            bin = get_heap()->bins[bin_idx];
            while (bin) {
                // Ensure we don't pick a TINY block for SMALL request (though size check handles it)
                // and ensure we don't pick a block that is too small
                if (UNFLAG(bin->size) >= size) {
                    remove_block_from_bins(bin);
                    bin->size &= ~FREE; // Mark allocated
                    bin->page->block_count++;
                    return fragment_block(bin, size);
                }
                bin = bin->next;
            }
            bin_idx++;
        }
    }

    // 2. No block found, request new page
    page = request_page(type, size);
    if (!page) return (NULL);

    // 3. For Large, the page *is* the block (kinda), handled in request_page
    // For Tiny/Small, request_page sets up a big free block.
    // We need to retrieve that free block.
    
    if (type == LARGE) {
        // Large pages have one block, already allocated logic-wise
        // We just return the first block
        bin = (t_block*)addr_offset(page, SIZEOF_PAGE);
        // It's already marked allocated in request_page logic usually, 
        // or we need to setup the block structure there.
        return bin;
    }

    // For TINY/SMALL, the page has one massive free block
    // We take it, fragment it, and return.
    bin = (t_block*)addr_offset(page, SIZEOF_PAGE);
    
    // It is currently in the bins (put there by request_page -> new_page)
    // We must remove it to use it.
    remove_block_from_bins(bin);
    bin->size &= ~FREE;
    bin->page->block_count++;
    
    return fragment_block(bin, size);
}

void    release_block(t_block* block)
{
    if (block == NULL) return;
    
    block->page->block_count--;
    
    // If page is empty, free it to system (optional but good for Large)
    // Subject says "limit munmap", but for LARGE we must munmap.
    // For TINY/SMALL, we can keep cached or free. Let's free if completely empty.
    if (block->page->block_count == 0) {
        release_page(block->page);
    } else {
        insert_block_to_bins(block);
    }
}