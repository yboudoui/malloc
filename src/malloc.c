#include "malloc.h"
#include "utils.h"

// Global singleton instance
t_heap g_heap = {0};

t_heap* get_heap(void) {
    return &g_heap;
}

inline size_t align(size_t size)
{
    if (size % ALIGNMENT == 0) return size;
    return size + (ALIGNMENT - (size % ALIGNMENT));
}

void*   malloc(size_t size)
{
    t_block* block;
    void     *addr;
    
    if (size == 0) return (NULL);
    size = align(size);
    block = request_block(size);
    if (block == NULL) return (NULL);
    addr = addr_offset(block, SIZEOF_BLOCK);
    return (addr);
}

void    free(void* addr)
{
    t_block* block;
    
    if (addr == NULL) return;
    block = addr_offset(addr, -SIZEOF_BLOCK);
    release_block(block);
}


void    *realloc(void *ptr, size_t size)
{
    t_block* block;
    void    *new_ptr;
    size_t  old_size;
    size_t  copy_size;

    if (ptr == NULL) return malloc(size);
    if (size == 0) return (free(ptr), NULL);

    block = addr_offset(ptr, -((long)SIZEOF_BLOCK));
    old_size = UNFLAG(block->size);

    if (align(size) == old_size) return ptr;

    new_ptr = malloc(size);
    if (!new_ptr) return (NULL);

    // FIX: Copy the smaller of the two sizes
    copy_size = (old_size < size) ? old_size : size;
    ft_memcpy(new_ptr, ptr, copy_size);
    
    free(ptr);
    return (new_ptr);
}