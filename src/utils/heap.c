#include "utils.h"

// Global singleton instance
t_heap g_heap = {0};

t_heap* get_heap(void) {
    return &g_heap;
}

t_block** get_bin_head(t_zone_type type)
{
    t_heap *heap = get_heap();
    if (type == TINY) return &heap->bins.tiny;
    if (type == SMALL) return &heap->bins.small;
    return &heap->bins.large;
}

t_page**  get_page_head(t_zone_type type)
{
    t_heap *heap = get_heap();
    if (type == TINY) return &heap->pages.tiny;
    if (type == SMALL) return &heap->pages.small;
    return &heap->pages.large;
}