#include "utils.h"
#include <sys/mman.h>
#include <unistd.h>

t_page**  get_page_head(t_zone_type type)
{
    t_heap *heap = get_heap();
    if (type == TINY) return &heap->pages.tiny;
    if (type == SMALL) return &heap->pages.small;
    return &heap->pages.large;
}

size_t   compute_mmap_size(t_zone_type type, size_t alloc_size)
{
    size_t page_size = sysconf(_SC_PAGESIZE);
    size_t size_req = 0;

    if (type == TINY) {
        // Subject Requirement: Pre-allocate space for at least 100 Tiny blocks
        size_req = 100 * (TINY_MAX + SIZEOF_BLOCK);
    } else if (type == SMALL) {
        // Subject Requirement: Pre-allocate space for at least 100 Small blocks
        size_req = 100 * (SMALL_MAX + SIZEOF_BLOCK);
    } else {
        // Large: Just the user size + headers
        size_req = alloc_size + SIZEOF_PAGE + SIZEOF_BLOCK;
    }

    // 1. Calculate how many pages we need (integer division rounding up)
    size_t pages_needed = (size_req + page_size - 1) / page_size;

    // 2. Return the total size in bytes
    return (pages_needed * page_size);
}

static void register_page(t_page *page)
{
    t_page **head = get_page_head(page->type);
    
    page->prev = NULL;
    page->next = *head;
    if (*head)
        (*head)->prev = page;
    *head = page;
}

static t_page *alloc_page(size_t size) 
{
    t_page*  page;

    page = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (page == MAP_FAILED) return (NULL);
    return (page);
}

static void init_page(t_page *page, t_zone_type type, size_t size)
{
    page->next = NULL;
    page->prev = NULL;
    page->size = size;
    page->block_count = 0;
    page->type = type;
}

t_page* request_page(t_zone_type type, size_t size)
{
    t_page*  page;
    t_block* block;
    size_t   map_size;

    map_size = compute_mmap_size(type, size);
    page = alloc_page(map_size);
    init_page(page, type, map_size);
    register_page(page);

    block = addr_offset(page, SIZEOF_PAGE);
    init_block(block, page, (map_size - SIZEOF_PAGE - SIZEOF_BLOCK));
    insert_block_to_bins(block);

    page->block_count += 1;
    return (page);
}

void    release_page(t_page* page)
{
    t_page **head;
    
    head = get_page_head(page->type);
    if (page->next) page->next->prev = page->prev;
    if (page->prev) page->prev->next = page->next;
    else *head = page->next;
    munmap((void*)page, page->size);
}