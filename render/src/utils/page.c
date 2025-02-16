#include "utils.h"

#include <sys/mman.h>
#include <unistd.h>

static size_t   compute_page_size(size_t request_size)
{
    size_t page_size;
    size_t num_pages;
    
    page_size = sysconf(_SC_PAGESIZE);
    num_pages = (request_size + page_size - 1) / page_size;
    return (num_pages * page_size);
}

static void     init_page(t_page page, size_t page_size)
{
    page->used_space = SIZEOF_PAGE;
    page->free_space = page_size - page->used_space;
    page->block_count = 0;
    page->next = NULL;
    page->prev = NULL;
    page->fake_prev_block_size = 0;
}

static t_page   new_page(size_t size)
{
    t_page page;
    size_t  page_size;

    page_size = compute_page_size(size);
    page = mmap(
        NULL, page_size,
        PROT_READ | PROT_WRITE,
        MAP_PRIVATE | MAP_ANONYMOUS,
        0, 0);
    if (page == MAP_FAILED)
        return (NULL);
    init_page(page, page_size);
    return (page);
}

t_page pages = NULL;

static t_page   find_page(size_t size)
{
    t_page page;

    page = pages;
    while (page && page->free_space < size)
        page = page->next;
    return (page);
}

static t_page   append_page(t_page page)
{
    if (page == NULL) return (NULL);
    page->next = pages;
    if (pages)
        pages->prev = page;
    pages = page;
    return (page);
}

static t_page   request_page(size_t size)
{
    t_page page = find_page(size);
    if (page != NULL) return (page);
    return (append_page(new_page(size)));
}

static size_t   acquire_memory_from_page(t_page page, size_t size)
{
    size_t  offset;
    
    offset = page->used_space;
    page->used_space += size;
    page->free_space -= size;
    return (offset);
}

static size_t   release_memory_from_page(t_page page, size_t size)
{
    size_t  offset;
    
    page->used_space -= size;
    page->free_space += size;
    offset = page->used_space;
    return (offset);
}

static t_block  init_block(t_block block, size_t size, size_t page_offset)
{
    block->prev_block_size |= 2;
    block->page_offset = page_offset;
    block->curr_block_size = size;
    set_tail_metadata(block, size);
    return (block);
}

t_block         request_new_block(size_t size)
{
    t_page  page;
    size_t  offset;

    page = request_page(SIZEOF_BLOCK + size + sizeof(size_t));
    if (page == NULL) return (NULL);
    release_memory_from_page(page, sizeof(size_t)); // for offsetting with the `prev_block_size`
    offset = acquire_memory_from_page(page, size);
    page->block_count += 1;
    return (init_block((t_block)(char*)page + offset, size, offset));
}

void    release_page(t_page page)
{
    if (page->prev)
        page->prev->next = page->next;
    if (page->next)
        page->next->prev = page->prev;
    munmap((void*)page, page->free_space + page->used_space);
}