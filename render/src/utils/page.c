#include "page.h"

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

static void     set_page(t_page page, size_t page_size)
{
    page->used_space = SIZEOF_PAGE;
    page->free_space = page_size - page->used_space;
    page->block_count = 0;
    page->next = NULL;
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
    set_page(page, page_size);
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
    if (page != NULL)
    {
        page->next = pages;
        pages = page;
    }
    return (page);
}

static t_page   request_page(size_t size)
{
    t_page page;

    page = find_page(size);
    if (page == NULL)
        page = append_page(new_page(size));
    return (page);
}

static size_t   compute_total_block_size(size_t size)
{
    return (SIZEOF_BLOCK + size + sizeof(size_t));
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

static t_block  acquire_block_from_page(t_page page, size_t size)
{
    size_t  offset;

    offset = acquire_memory_from_page(page, size);
    page->block_count += 1;
    return (set_block((char*)page + offset, size, offset));
}

void            release_block(t_block block)
{
    t_page  page;

    page = (t_page)((char*)block - block->page_offset);
    page->block_count -= 1;
}

t_block         request_block(size_t size)
{
    t_page  page;
    size_t  total_size;
    size_t  offset;

    total_size = compute_total_block_size(size);
    page = request_page(total_size);
    if (page == NULL) return (NULL);
    // for offsetting with the `prev_block_size`
    release_memory_from_page(page, sizeof(size_t));
    return (acquire_block_from_page(page, size));
}