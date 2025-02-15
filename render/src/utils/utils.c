#include "block.h"

static t_block  get_next_block(t_block block)
{
    size_t  *tail_metadata;

    tail_metadata = get_tail_metadata(block);
    if ((*tail_metadata) & 2)
        return ((char*)tail_metadata + sizeof(size_t));
    return (NULL);
}

static t_block  get_next_free_block(t_block block)
{
    t_block next_block;

    next_block = get_next_block(block);
    if (next_block && (next_block->curr_block_size & 1))
        return (next_block);
    return (NULL);
}

static t_block  forward_coalesce(t_block block)
{
    t_block next_free_block;

    next_free_block = get_next_free_block(block);
    while (next_free_block)
    {
        block->curr_block_size += sizeof(size_t) + SIZEOF_BLOCK;
        block->curr_block_size += next_free_block->curr_block_size;
        set_tail_metadata(block, block->curr_block_size);
        next_free_block = get_next_free_block(block);
    }
    return (block);
}


t_block*    get_prev_block(t_block *block)
{
    char    *ptr;

    ptr = (char*)block;
    ptr -= block->prev_size;
    ptr -= sizeof(t_block);
    return ((t_block*)ptr);
}

t_block*    backward_coalescence(t_block *block)
{
    t_block *prev_block;

    if (is_free(block->prev_size) == 0)
        return (block);
    prev_block = get_prev_block(block);
    block = backward_coalescence(prev_block);
    block->size += next_block->size + sizeof(t_block); // Increase size
    block->next_free = next_block->next; // Link next free block
}

t_block* coalesce(t_block* block)
{
    block = forward_coalescence(block);
    block = backward_coalescence(block);
    return (block);
}