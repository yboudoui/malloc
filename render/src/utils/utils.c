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
    t_size    *tail_metadata;

    tail_metadata = get_tail_metadata(block);
    if (tail_metadata->flag.flag & 2)
        return ((t_block)tail_metadata);
    return (NULL);
}

static t_block  get_prev_block(t_block block)
{
    t_block prev_block;
    size_t  prev_size;
    char    *ptr;

    prev_size = get_unflaged_size(block->prev_block_size);
    if (prev_size == 0) return (NULL);
    ptr = (char*)block;
    ptr -= prev_size;
    ptr -= SIZEOF_BLOCK;
    prev_block = (t_block)ptr;
    return (prev_block);
}

t_block  is_block_free(t_block block)
{
    if (block && block->curr_block_size.flag.flag & 1)
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
    size_t  dst_size;
    size_t  src_size;
    t_size  *src_tail_metadata;
    t_size  *dst_tail_metadata;
    size_t  size;
    
    dst_size = get_unflaged_size(dest->curr_block_size);
    src_size = get_unflaged_size(src->curr_block_size);
    src_tail_metadata = get_tail_metadata(src);

    size = dst_size + SIZEOF_BLOCK + src_size;
    dest->curr_block_size.flag.flag = src_tail_metadata->flag.flag;
    dst_tail_metadata = get_tail_metadata(dest);
    dst_tail_metadata->raw = dest->curr_block_size.raw;
    remove_block(src);
    release_block_from_page(src);
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