#include "utils.h"

t_block** get_bin_head(t_zone_type type)
{
    t_heap *heap = get_heap();
    if (type == TINY) return &heap->bins.tiny;
    if (type == SMALL) return &heap->bins.small;
    return &heap->bins.large;
}

static t_block* find_free_block(t_zone_type type, size_t size)
{
    t_block *block = *get_bin_head(type);

    while (block) {
        if (UNFLAG(block->size) >= size)
            return (block);
        block = block->next;
    }
    return (NULL);
}

static t_zone_type get_size_type(size_t size)
{
    if (size <= TINY_MAX) return TINY;
    if (size <= SMALL_MAX) return SMALL;
    return LARGE;
}

t_block* remove_block_from_bins(t_block* block)
{
    if (!block) return (NULL);
    
    if (block->next) block->next->prev = block->prev;
    if (block->prev) block->prev->next = block->next;
    
    t_block **head = get_bin_head(block->page->type);
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

    t_block **head = get_bin_head(block->page->type);

    block->prev = NULL;
    block->next = *head;
    if (*head) {
        (*head)->prev = block;
    }
    *head = block;
}

t_block* request_block(size_t size)
{
    t_block*    block;
    t_zone_type type;

    type = get_size_type(size);

    if (type == LARGE) {
        t_page *page = request_page(LARGE, size);
        if (!page) return (NULL);
        return (t_block*)addr_offset(page, SIZEOF_PAGE);
    }
    
    block = find_free_block(type, size);

    if (!block) {
        if (!request_page(type, size)) return (NULL);
        block = find_free_block(type, size);
    }

    return fragment_block(block, size);
}

void    release_block(t_block* block)
{
    if (!block) return;

    block->page->block_count--;

    if (block->page->type == LARGE) {
        release_page(block->page);
        return;
    }

    // 1. Mark as FREE
    block->size |= FREE;
    
    // 2. Merge neighbors (removes neighbors from bins if used)
    block = coalesce_block(block);
    
    // 3. Insert the final big block into the bin
    insert_block_to_bins(block);
}