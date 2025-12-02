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

static void page_add_front(t_page *page)
{
    t_page **head = get_page_head(page->type);
    
    page->prev = NULL;
    page->next = *head;
    if (*head)
        (*head)->prev = page;
    *head = page;
}

t_page* request_page(t_zone_type type, size_t size)
{
    t_page*  page;
    t_block* block;
    size_t   map_size;

    // A. Allocate Memory
    map_size = compute_mmap_size(type, size);
    page = mmap(NULL, map_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (page == MAP_FAILED) return (NULL);

    // B. Initialize Page Header
    page->type = type;
    page->block_count = 0;
    page_add_front(page);

    // C. Initialize the First Block
    block = (t_block*)addr_offset(page, SIZEOF_PAGE);
    block->page = page;
    block->size = (map_size - SIZEOF_PAGE - SIZEOF_BLOCK) | FREE;
    // block->next/prev/prev_block are 0 because mmap zeros memory

    // D. Final Setup
    if (type == LARGE) {
        block->size &= ~FREE; // Mark allocated immediately
        page->block_count = 1;
    } else {
        insert_block_to_bins(block); // Add to free list
    }

    return (page);
}

void    release_page(t_page* page)
{
    // 1. Unlink page from the list
    t_page **head = get_page_head(page->type);
    if (page->next) page->next->prev = page->prev;
    if (page->prev) page->prev->next = page->next;
    else *head = page->next;

    // 2. Determine the size to unmap
    size_t alloc_size = 0;
    
    if (page->type == LARGE) {
        // For LARGE, the size depends on the block size
        t_block *block = (t_block*)addr_offset(page, SIZEOF_PAGE);
        alloc_size = UNFLAG(block->size);
    }
    // For TINY/SMALL, alloc_size is ignored by compute_mmap_size 
    // because it uses the fixed * 100 constants.

    // 3. Re-use the calculation logic (Guarantees consistency)
    size_t map_size = compute_mmap_size(page->type, alloc_size);

    munmap((void*)page, map_size);
}