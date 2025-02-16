#include "utils.h"

t_page   get_page_from_block(t_block block)
{
    char    *addr;

    addr = (char*)block;
    addr -= block->page_offset;
    return ((t_page)addr);
}

t_block  get_next_block(t_block block)
{
    size_t  *tail_metadata;

    tail_metadata = get_tail_metadata(block);
    if ((*tail_metadata) & 2)
        return ((t_block)tail_metadata);
    return (NULL);
}

static t_block  get_prev_block(t_block block)
{
    size_t  prev_size;
    char    *ptr;

    prev_size = get_unflaged_size(block->prev_block_size);
    if (prev_size == 0) return (NULL);
    ptr = (char*)block;
    ptr -= prev_size - SIZEOF_BLOCK;
    return ((t_block)ptr);
}

t_block  is_block_free(t_block block)
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