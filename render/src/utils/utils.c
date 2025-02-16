#include "utils.h"

t_page   get_page_from_block(t_block block)
{
    char    *addr;

    addr = (char*)block;
    addr -= block->page_offset;
    return ((t_page)addr);
}

static t_block  get_next_block(t_block block)
{
    t_block next_block;

    next_block = (t_block)get_tail_metadata(block);
    if (next_block->prev_block_size & 2)
        return (next_block);
    return (NULL);
}

static t_block  get_prev_block(t_block block)
{
    size_t  prev_size;

    prev_size = get_unflaged_size(block->prev_block_size);
    if (prev_size == 0)
        return (NULL);
    return ((t_block)(char*)block - prev_size - SIZEOF_BLOCK);
}

static t_block  is_block_free(t_block block)
{
    if (block && (block->curr_block_size & 1))
        return (block);
    return (NULL);
}

static void     remove_block(t_block block)
{
    if (block->prev)
        block->prev->next = block->next;
    if (block->next)
        block->next->prev = block->prev;
}

static t_block  merge_block(t_block src, t_block dest)
{
    t_page  page;

    dest->curr_block_size += SIZEOF_BLOCK + src->curr_block_size;
    set_tail_metadata(dest, dest->curr_block_size);
    remove_block(src);
    page = get_page_from_block(src);
    page->block_count -= 1;
    return (dest);
}

t_block coalesce(t_block block)
{
    t_block next;
    t_block prev;

    next = is_block_free(get_next_block(block));
    while (next) {
        block = merge_block(next, block);
        next = is_block_free(get_next_block(block));
    }

    prev = is_block_free(get_prev_block(block));
    while (prev) {
        block = merge_block(block, prev);
        prev = is_block_free(get_prev_block(block));
    }

    return (block);
}