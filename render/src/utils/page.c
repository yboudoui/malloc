#include "utils.h"

#include <sys/mman.h>
#include <unistd.h>

t_page  global_pages(t_page page)
{
    static struct s_page pages = {};

    if (page != NULL)
    {
        pages.next = page;
        pages.next->prev = &pages;
    }
    return (pages.next);
}

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

static void     init_block(t_block block, size_t size, size_t page_offset)
{
    block->prev_block_size |= 2;
    block->page_offset = page_offset;
    set_tail_metadata(block, size);
    block->curr_block_size = size | 2;
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

static t_page   find_page(size_t size)
{
    t_page page;

    page = global_pages(NULL);
    while (page && page->free_space < size)
        page = page->next;
    return (page);
}

static t_page   append_page(t_page page)
{
    if (page == NULL) return (NULL);
    page->next = global_pages(NULL);
    if (page->next) page->next->prev = page;
    return (global_pages(page));
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

/*static*/ size_t   release_memory_from_page(t_page page, size_t size)
{
    size_t  offset;
    
    page->used_space -= size;
    page->free_space += size;
    offset = page->used_space;
    return (offset);
}

static t_block  acquire_block_from_page(t_page page, size_t block_size)
{
    size_t  offset;
    t_block block;

    page->block_count += 1;
    offset = acquire_memory_from_page(page, SIZEOF_BLOCK + block_size);
    offset -= sizeof(size_t);
    block = (t_block)((char*)page + offset);
    init_block(block, block_size, offset);
    return (block);
}

void  release_block_from_page(t_block block)
{
    t_page  page;
    // size_t  size;

    page = get_page_from_block(block);
    page->block_count -= 1;
    // size = get_unflaged_size(block->curr_block_size);
    // release_memory_from_page(page, SIZEOF_BLOCK + size);
}

t_block         request_new_block(size_t size)
{
    t_page  page;

    page = request_page(SIZEOF_BLOCK + size);
    if (page == NULL) return (NULL);
    return (acquire_block_from_page(page, size));
}

void    release_page(t_page page)
{
    if (page->prev)
        page->prev->next = page->next;
    if (page->next)
        page->next->prev = page->prev;
    munmap((void*)page, page->free_space + page->used_space);
}