#include "utils.h"
#include <sys/mman.h>
#include <unistd.h>

static size_t   compute_zone_size(t_zone_type type, size_t alloc_size)
{
    size_t page_size = sysconf(_SC_PAGESIZE);
    size_t size_req = 0;

    if (type == TINY) {
        // (128 + Header) * 100
        size_req = 100 * (TINY_MAX + SIZEOF_BLOCK);
    } else if (type == SMALL) {
        // (1024 + Header) * 100
        size_req = 100 * (SMALL_MAX + SIZEOF_BLOCK);
    } else {
        // Large: just the alloc size + page header + block header
        size_req = alloc_size + SIZEOF_PAGE + SIZEOF_BLOCK;
    }

    // Round up to multiple of page_size
    size_t num_pages = (size_req + page_size - 1) / page_size;
    return (num_pages * page_size);
}

t_page*   request_page(t_zone_type type, size_t size)
{
    t_page*  page;
    size_t   total_size;
    t_block* first_block;

    total_size = compute_zone_size(type, size);

    page = mmap(NULL, total_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (page == MAP_FAILED) return (NULL);

    ft_bzero(page, SIZEOF_PAGE);
    page->type = type;
    page->block_count = 0;

    // Link into global list
    page->next = get_heap()->pages[type];
    if (page->next) page->next->prev = page;
    get_heap()->pages[type] = page;

    // Initialize the first block covering the whole page (minus page header)
    size_t data_size = total_size - SIZEOF_PAGE - SIZEOF_BLOCK;
    
    first_block = (t_block*)addr_offset(page, SIZEOF_PAGE);
    first_block->size = data_size | FREE;
    first_block->page = page;
    first_block->prev_block = NULL;
    first_block->next = NULL;
    first_block->prev = NULL;

    // If Large, we treat it as allocated immediately to avoid bin logic
    if (type == LARGE) {
        first_block->size &= ~FREE;
        page->block_count = 1;
    } else {
        // For Tiny/Small, put the massive block into bins
        insert_block_to_bins(first_block);
    }

    return (page);
}

void    release_page(t_page* page)
{
    t_block* block;
    size_t   map_size;

    // Unlink from global list
    if (page->next) page->next->prev = page->prev;
    if (page->prev) page->prev->next = page->next;
    else get_heap()->pages[page->type] = page->next;

    // For TINY/SMALL, we need to remove the big free block from bins 
    // before unmapping, otherwise bins point to invalid memory.
    if (page->type != LARGE) {
        block = (t_block*)addr_offset(page, SIZEOF_PAGE);
        // Coalesce everything first to get one big block?
        // Actually, if page block_count is 0, it means all blocks are free.
        // But they might be fragmented.
        // Since we are unmapping, we must just iterate the physical blocks
        // and remove them from bins if they are free.
        block = (t_block*)addr_offset(page, SIZEOF_PAGE);
        while (block) {
            if (block->size & FREE) {
                remove_block_from_bins(block);
            }
            block = get_next_block(block);
            // Safety break if we go out of page bounds (requires checking size)
             if ((void*)block >= addr_offset(page, compute_zone_size(page->type, 0))) break;
        }
    }

    map_size = compute_zone_size(page->type, 0);
    if (page->type == LARGE) {
         // Re-calculate based on block size for large because it's variable
         block = (t_block*)addr_offset(page, SIZEOF_PAGE);
         map_size = UNFLAG(block->size) + SIZEOF_PAGE + SIZEOF_BLOCK;
         // Round up to page size
         size_t sys_page = sysconf(_SC_PAGESIZE);
         map_size = (map_size + sys_page - 1) & ~(sys_page - 1);
    }

    munmap((void*)page, map_size);
}