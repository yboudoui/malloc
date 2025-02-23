#include "utils.h"

#include <sys/mman.h>
#include <unistd.h>

t_page*          global_pages(t_page* page)
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

#define SIZEOF_METADATA_PAGE (SIZEOF_PAGE + SIZEOF_BLOCK)
static t_page*   new_page(size_t size)
{
    t_page*  page;
    size_t  page_size;

    page_size = compute_page_size(SIZEOF_METADATA_PAGE + size);
    page = mmap(
        NULL, page_size,
        PROT_READ | PROT_WRITE,
        MAP_PRIVATE | MAP_ANONYMOUS,
        0, 0);
    if (page == MAP_FAILED) return (NULL);
    ft_bzero(page, SIZEOF_PAGE);
    page->block_count = 1;
    page->free.page = page;
    page->free.size = (page_size - SIZEOF_METADATA_PAGE) | FREE;
    page->free.prev_block = NULL;
    return (page);
}

t_page*   request_page(size_t size)
{
    t_page* page;

    page = global_pages(NULL);
    while (page && UNFLAG(page->free.size) < size) page = page->next;
    if (page != NULL) return (page);
    page = new_page(size);
    if (page == NULL) return (NULL);
    page->next = global_pages(NULL);
    if (page->next) page->next->prev = page;
    return (global_pages(page));
}

void    release_page(t_page* page)
{
    // remove_block_from_bins(&page->free_space);
    if (page->next) page->next->prev = page->prev;
    if (page->prev) page->prev->next = page->next;
    munmap((void*)page, SIZEOF_METADATA_PAGE + UNFLAG(page->free.size));
}