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
    t_block *block;
    size_t blk_size;
    
    block = *get_bin_head(type);
    while (block) {
        blk_size = get_block_size(block);
        if (blk_size + SIZEOF_BLOCK + ALIGNMENT >= size) return (block);
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
    t_block **head;

    if (block == NULL) return (NULL);
    
    if (block->next) block->next->prev = block->prev;
    if (block->prev) block->prev->next = block->next;
    
    head = get_bin_head(block->page->type);
    if (*head == block) *head = block->next;

    block->next = NULL;
    block->prev = NULL;
    set_block_not_free(block);
    return (block);
}

void    insert_block_to_bins(t_block* block)
{
    t_block **head;

    if (block == NULL) return;
    set_block_free(block);

    head = get_bin_head(block->page->type);
    block->prev = NULL;
    block->next = *head;
    if (*head) (*head)->prev = block;
    *head = block;
}

void    set_block_free(t_block *block)
{
    block->size |= FREE;
}

void    set_block_not_free(t_block *block)
{
    block->size &= ~FREE;
}


size_t  get_block_size(t_block *block)
{
    return (UNFLAG(block->size));
}

void    init_block(t_block *block, t_page *page, size_t size)
{
    block->prev_block = NULL;
    block->size = size;
    block->page = page;
    block->next = NULL;
    block->prev = NULL;
}

t_block* request_block(size_t size)
{
    t_block*    block;
    t_page*     page;
    t_zone_type type;

    type = get_size_type(size);
    block = find_free_block(type, size);
    if (block == NULL) {
        page = request_page(type, size);
        if (page == NULL) return (NULL);
        block = find_free_block(type, size);
    }

    return fragment_block(block, size);
}

void    release_block(t_block* block)
{
    if (block == NULL) return;
    if (block->page->type == LARGE) {
        release_page(block->page);
        return;
    }
    block = coalesce_block(block);
    insert_block_to_bins(block);
}