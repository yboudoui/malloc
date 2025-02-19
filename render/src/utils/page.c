#include "utils.h"

#include <sys/mman.h>
#include <unistd.h>

t_page  global_pages(t_page page)
{
    static struct s_page pages = {};

    if (page == NULL) return (pages.next);
    page->prev = &pages;
    page->next = pages.next;
    if (page->next) page->next->prev = page;
    return (page);
}

static size_t   compute_page_size(size_t request_size)
{
    size_t page_size;
    size_t num_pages;
    
    page_size = sysconf(_SC_PAGESIZE);
    num_pages = (request_size + page_size - 1) / page_size;
    return (num_pages * page_size);
}

static void init_block(t_block block, size_t size, size_t page_offset)
{
    block->page_offset = page_offset;
    set_block_size(block, size);
}

static void     init_page(t_page page, size_t page_size)
{
    page->next = NULL;
    page->prev = NULL;
    page->block_count = 1;
    page->used_space = SIZEOF_PAGE + SIZEOF_BLOCK;
    init_block(
        &page->free_space,
        page_size - page->used_space,
        SIZEOF_PAGE - SIZEOF_BLOCK);
    get_next_block(&page->free_space)->prev_block_size = 0;
    set_block_flag(&page->free_space, FREE);
}

static t_page   new_page(size_t size)
{
    t_page page;
    size_t  page_size;

    page_size = compute_page_size(SIZEOF_PAGE + size + SIZEOF_BLOCK);
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
    while (page && UNFLAG(page->free_space.size) < size)
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

static t_block  fragment_block(t_block block, size_t size)
{
    size_t  flag;
    t_block new_block;
    size_t  new_size;

    new_size = (UNFLAG(block->size) - (SIZEOF_BLOCK + size));

    flag = FLAG(block->size);

    new_block = addr_offset(block, new_size + SIZEOF_BLOCK);
    init_block(new_block, size, block->page_offset + new_size);
    init_block(block, new_size, block->page_offset);
    set_block_flag(block, flag);
    new_block->prev_block_size = block->size;
    return (new_block);
}

t_block         request_new_block(size_t size)
{
    t_page  page;

    page = request_page(SIZEOF_BLOCK + size);
    if (page == NULL) return (NULL);
    page->block_count += 1;
    page->used_space += size;
    return (fragment_block(&page->free_space, size));
}

void    release_page(t_page page)
{
    remove_block(&page->free_space);
    if (page->next) page->next->prev = page->prev;
    if (page->prev) page->prev->next = page->next;
    munmap((void*)page,
        UNFLAG(page->free_space.size) + page->used_space);
}