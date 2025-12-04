#include "malloc.h"
#include "utils.h"
#include <unistd.h>

/* -------------------------------------------------------------------------- */
/*                                 I/O HELPERS                                */
/* -------------------------------------------------------------------------- */

static void ft_putstr(const char *s)
{
    const char *p;

    if (!s) return;
    p = s;
    while (*p) p++;
    write(1, s, p - s);
}

static void ft_putnbr(size_t n)
{
    char c;

    if (n >= 10) ft_putnbr(n / 10);
    c = (n % 10) + '0';
    write(1, &c, 1);
}

static void ft_puthex(size_t n)
{
    char *base = "0123456789ABCDEF";

    if (n >= 16) ft_puthex(n / 16);
    write(1, &base[n % 16], 1);
}

static void print_addr(void *addr)
{
    write(1, "0x", 2);
    ft_puthex((size_t)addr);
}

/* -------------------------------------------------------------------------- */
/*                              BLOCK PRINTING                                */
/* -------------------------------------------------------------------------- */

static void print_block_info(t_block *block)
{
    void *start_addr;
    void *end_addr;
    size_t size;

    size = UNFLAG(block->size);
    start_addr = addr_offset(block, SIZEOF_BLOCK);
    end_addr = addr_offset(start_addr, size);

    print_addr(start_addr);
    ft_putstr(" - ");
    print_addr(end_addr);
    ft_putstr(" : ");
    ft_putnbr(size);
    ft_putstr(" bytes\n");
}

/* -------------------------------------------------------------------------- */
/*                              PAGE TRAVERSAL                                */
/* -------------------------------------------------------------------------- */

// Calculate the safe end of the memory mapped region to prevent segfaults
static void *get_page_limit(t_page *page)
{
    size_t raw_size;
    size_t page_size = sysconf(_SC_PAGESIZE);

    if (page->type == TINY)
        raw_size = 100 * (TINY_MAX + SIZEOF_BLOCK);
    else if (page->type == SMALL)
        raw_size = 100 * (SMALL_MAX + SIZEOF_BLOCK);
    else {
        // For Large, read the block size roughly
        t_block *b = (t_block*)addr_offset(page, SIZEOF_PAGE);
        raw_size = UNFLAG(b->size) + SIZEOF_BLOCK + SIZEOF_PAGE;
    }

    // Round up to system page size
    size_t aligned_size = (raw_size + page_size - 1) & ~(page_size - 1);
    return addr_offset(page, aligned_size);
}

static size_t print_blocks_in_page(t_page *page)
{
    t_block *block;
    void    *limit;
    size_t  total_bytes;

    total_bytes = 0;
    block = (t_block*)addr_offset(page, SIZEOF_PAGE);
    limit = get_page_limit(page);

    while ((void*)block < limit)
    {
        // Safety check: Ensure the header is fully within bounds
        if ((void*)block + SIZEOF_BLOCK > limit) break;

        // If allocated, print it
        if (!(block->size & FREE)) {
            print_block_info(block);
            total_bytes += UNFLAG(block->size);
        }

        // Get next block and check boundary
        t_block *next = get_next_block(block);
        
        // If next is NULL (end of logic) or pointer is out of bounds
        if (!next || (void*)next > limit) break;
        block = next;
    }
    return (total_bytes);
}

/* -------------------------------------------------------------------------- */
/*                              ZONE LOGIC                                    */
/* -------------------------------------------------------------------------- */

static size_t print_zone(t_zone_type type)
{
    t_page *page;
    size_t total_allocated;
    char   *zone_name;

    // 1. Determine Zone Name and List Head
    if (type == TINY) {
        zone_name = "TINY";
        page = get_heap()->pages.tiny;
    } else if (type == SMALL) {
        zone_name = "SMALL";
        page = get_heap()->pages.small;
    } else {
        zone_name = "LARGE";
        page = get_heap()->pages.large;
    }

    // 2. Print Zone Header
    ft_putstr(zone_name);
    ft_putstr(" : ");
    print_addr(page); // Print address of first page (or 0x0)
    write(1, "\n", 1);

    // 3. Iterate Pages
    total_allocated = 0;
    while (page)
    {
        total_allocated += print_blocks_in_page(page);
        page = page->next;
    }
    
    return (total_allocated);
}

/* -------------------------------------------------------------------------- */
/*                              MAIN ENTRY POINT                              */
/* -------------------------------------------------------------------------- */

void    show_alloc_mem(void)
{
    size_t total;

    total = 0;
    total += print_zone(TINY);
    total += print_zone(SMALL);
    total += print_zone(LARGE);

    ft_putstr("Total : ");
    ft_putnbr(total);
    ft_putstr(" bytes\n");
}