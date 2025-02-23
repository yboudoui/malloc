#include "malloc.h"
#include "utils.h"

static inline size_t align(size_t size)
{
    return ((size + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1));
}

void*   malloc(size_t size)
{
    t_block* block;
    
    if (size == 0) return (0);
    block = request_block(align(size));
    return (addr_offset(block, SIZEOF_BLOCK));
}

void    free(void* addr)
{
    t_block* block;
    
    if (addr == NULL) return;
    block = addr_offset(addr, -SIZEOF_BLOCK);
    release_block(coalesce_block(block));
}

void	*memcpy(void *dest, const void *src, size_t n)
{
	unsigned char	*d;
	unsigned char	*s;

	d = (unsigned char *)dest;
	s = (unsigned char *)src;
	if (!d || !s)
		return (dest);
	while (n--)
		*d++ = *s++;
	return (dest);
}
/*
void    *realloc(void *ptr, size_t size)
{
    t_block* block;
    void    *new_addr;
    size_t  old_size;

    block = get_block_from_addr(ptr, -SIZEOF_BLOCK);
    if (block == NULL) return (NULL);
    new_addr = malloc(size);
    if (new_addr == NULL) return (NULL);
    old_size = get_unflaged_size(block->curr_block_size);
    if (old_size < size)
        size = old_size;
    memcpy(new_addr, ptr, size);
    return (free(ptr), new_addr);
}
*/