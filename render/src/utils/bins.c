#include "utils.h"

struct s_block bins[MAX_BINS] = {};

static t_block* get_bin_by_size(size_t size)
{
    if (size > MAX_FAST_SIZE) size -= MAX_FAST_SIZE;
    size /= ALIGNMENT;
    size = size > 127 ? 127 : size;
    return (&bins[size]);
}

t_block* remove_block_from_bins(t_block* block)
{
    if (!block) return(NULL);
    if (block->next) block->next->prev = block->prev;
    if (block->prev) block->prev->next = block->next;
    block->next = NULL;
    block->prev = NULL;
    return (block);
}

void    insert_after(t_block* current, t_block* new_block)
{
    if (!current || !new_block) return; 

    new_block->next = current->next;
    new_block->prev = current;

    if (current->next) current->next->prev = new_block;
    current->next = new_block;
}

void    release_block(t_block* block)
{
    if (block == NULL) return;
    block->page->block_count -= 1;
    if (block->page->block_count == 1) return (release_page(block->page));
    insert_after(get_bin_by_size(UNFLAG(block->size)), block);
}

t_block* request_block(size_t size)
{
    t_block* bin;
    t_page*  page;

    bin = get_bin_by_size(size);
    while (bin && UNFLAG(bin->size) < size) bin = bin->next;

    bin = remove_block_from_bins(bin);
    if (bin) {
        bin->size &= ~FREE;
        bin->page->block_count += 1;
        return (bin);
    }
    page = request_page(SIZEOF_BLOCK + size);
    if (page == NULL) return (NULL);
    bin = fragment_block(&page->free, size);
    if (bin) {
        bin->size &= ~FREE;
        bin->page->block_count += 1;
    }
    return (bin);
}
