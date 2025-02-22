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
    pages.next = page;
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
    ft_bzero(page, SIZEOF_PAGE);
    page->block_count = 1;
    page->used_space = SIZEOF_PAGE + sizeof(size_t);
    page->free_space.page_offset = SIZEOF_PAGE - SIZEOF_BLOCK;
    page->free_space.size = (page_size - page->used_space) | FREE;
    get_next_block(&page->free_space)->prev_block_size = page->free_space.size;
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

t_block         fragment_block(t_block block, size_t size)
{
    t_block new_block;

    remove_block_from_bins(block);
    if (UNFLAG(block->size) == (SIZEOF_BLOCK + size)) return (block);
    block->size = FLAG(block->size) | (UNFLAG(block->size) - (SIZEOF_BLOCK + size));

    new_block = ft_bzero(get_next_block(block), SIZEOF_BLOCK);
    new_block->prev_block_size = block->size;
    new_block->size = size;
    new_block->page_offset = block->page_offset + (SIZEOF_BLOCK + UNFLAG(block->size));
    return (new_block);
}

t_block         request_new_block(size_t size)
{
    t_page  page;
    t_block new_block;

    page = request_page(SIZEOF_BLOCK + size);
    if (page == NULL) return (NULL);
    new_block = fragment_block(&page->free_space, size);
    page->block_count += 1;
    page->used_space += SIZEOF_BLOCK + size;
    return (new_block);
}

void    release_page(t_page page)
{
    // remove_block_from_bins(&page->free_space);
    if (page->next) page->next->prev = page->prev;
    if (page->prev) page->prev->next = page->next;
    munmap((void*)page,
        UNFLAG(page->free_space.size) + page->used_space);
}