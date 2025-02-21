#include "utils.h"

struct s_block bins[MAX_BINS] = {};

static size_t get_bin_index(size_t size)
{
    if (size > MAX_FAST_SIZE)
        size -= MAX_FAST_SIZE;
    size /= ALIGNMENT;
    return (size > 127 ? 127 : size);
}

t_block remove_block(t_block block)
{
    if (!block) return(NULL);
    if (block->next) block->next->prev = block->prev;
    if (block->prev) block->prev->next = block->next;
    block->next = NULL;
    block->prev = NULL;
    return (block);
}

void    insert_after(t_block current, t_block new_block)
{
    if (!current || !new_block) return; 

    new_block->next = current->next;
    new_block->prev = current;

    if (current->next) current->next->prev = new_block;
    current->next = new_block;
}

void    release_block(t_block block)
{
    size_t  index;

    index = get_bin_index(UNFLAG(block->size));
    insert_after(&bins[index], block);
}

t_block request_available_block(size_t size)
{
    size_t  index;

    index = get_bin_index(size);
    return (remove_block(bins[index].next));
}

