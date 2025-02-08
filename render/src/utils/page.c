#include "page.h"

#include <sys/mman.h>
#include <unistd.h>

static size_t  compute_page_size(size_t request_size)
{
    size_t page_size;
    size_t num_pages;
    
    page_size = getpagesize();
    num_pages = (request_size + page_size - 1) / page_size;
    return (num_pages * page_size);
}

static t_page* new_page(size_t size)
{
    t_page* page;
    size_t  page_size;

    page_size = compute_page_size(size);
    page = mmap(
        NULL, page_size,
        PROT_READ | PROT_WRITE,
        MAP_PRIVATE | MAP_ANONYMOUS,
        0, 0);
    if (page == MAP_FAILED)
        return (NULL);
    page->total_space = page_size - sizeof(t_page);
    page->free_space = page->total_space;
    return (page);
}

t_page* pages = NULL;

static t_page*  find_page(size_t size)
{
    t_page* page;

    page = pages;
    while (page && page->free_space < size)
        page = page->next;
    return (page);
}

static t_page*  append_page(t_page *page)
{
    if (page != NULL)
    {
        page->next = pages;
        pages = page;
    }
    return (page);
}

static t_page*  request_page(size_t size)
{
    t_page* page;

    page = find_page(size);
    if (page == NULL)
        page = append_page(new_page(size));
    return (page);
}

static void*    get_next_free_addr(t_page* page)
{
    char*   addr;

    if (page == NULL)
        return (NULL);
    addr = (char*)page + sizeof(t_page);
    addr += (page->total_space - page->free_space);
    return (addr);
}

void* request_memory_from_page(size_t size)
{
    t_page* page;
    void*   addr;

    page = request_page(size);
    if (page == NULL)
        return (NULL);
    addr = get_next_free_addr(page);
    page->free_space -= size;
    page->block_count += 1;
    return (addr);
}