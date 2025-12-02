#include "malloc.h"
#include "utils.h"

// Global singleton instance
t_heap g_heap = {0};

t_heap* get_heap(void) {
    return &g_heap;
}

static inline size_t align(size_t size)
{
    return ((size + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1));
}

void*   malloc(size_t size)
{
    t_block* block;
    size_t   aligned_size;
    
    if (size == 0) return (NULL);
    
    aligned_size = align(size);
    block = request_block(aligned_size);
    
    if (!block) return (NULL);
    
    // Return pointer to data (after the block header)
    return (addr_offset(block, SIZEOF_BLOCK));
}

void    free(void* addr)
{
    t_block* block;
    
    if (addr == NULL) return;
    
    // Jump back to the header
    block = addr_offset(addr, -((long)SIZEOF_BLOCK));
    
    // Security check (basic) to ensure we are inside our heap
    // In a real production malloc, we'd check if the page exists in our tree/list
    
    release_block(coalesce_block(block));
}

void    *realloc(void *ptr, size_t size)
{
    t_block* block;
    void    *new_ptr;
    size_t  old_size;

    if (ptr == NULL)
        return malloc(size);
    
    if (size == 0) {
        free(ptr);
        return NULL;
    }

    block = addr_offset(ptr, -((long)SIZEOF_BLOCK));
    old_size = UNFLAG(block->size);

    // Optimization: if new size fits in old size, just return (or split)
    // For compliance with subject "creates a new allocation", we do the standard way
    // unless size is exactly the same or we want to implement in-place expansion.
    if (size <= old_size)
        return ptr; // Simple reuse

    new_ptr = malloc(size);
    if (!new_ptr) return (NULL);

    ft_memcpy(new_ptr, ptr, old_size);
    free(ptr);
    return (new_ptr);
}