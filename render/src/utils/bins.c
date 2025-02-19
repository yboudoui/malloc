#include "utils.h"

struct s_block bins[MAX_BINS] = {};

static size_t get_bin_index(size_t size)
{
    if (size > MAX_FAST_SIZE)
        size -= MAX_FAST_SIZE;
    size /= ALIGNMENT;
    return (size > 126 ? 127 : size);
}

t_block remove_block(t_block block)
{
    if (block->next) block->next->prev = block->prev;
    if (block->prev) block->prev->next = block->next;
    block->next = NULL;
    block->prev = NULL;
    return (block);
}

void    release_block(t_block block)
{
    t_block bin;

    bin = bins + get_bin_index(UNFLAG(block->size));

    block->prev = bin;
    block->next = bin->next;
    bin->next = block;

    get_page_from_block(block)->block_count -= 1;
    set_block_flag(block, FREE);
}

t_block request_available_block(size_t size)
{
    t_block bin;
    t_block block;

    bin = bins + get_bin_index(size);

    block = bin->next;
    if (block == NULL) return (NULL);
    bin->next = block->next;
    if (bin->next) bin->next->prev = bin;

    block->next = NULL;
    block->prev = NULL;
    get_page_from_block(block)->block_count += 1;
    return (unset_block_flag(block, FREE));
}

