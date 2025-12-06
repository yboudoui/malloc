#include "utils.h"

void*   addr_offset(void *addr, long offset)
{
    if (addr == NULL) return NULL;
    return (void*)((uintptr_t)addr + offset);
}

void    *ft_bzero(void* addr, size_t size)
{
    unsigned char *ptr = addr;
    while (size--) *ptr++ = 0;
    return (addr);
}

void	*ft_memcpy(void *dest, const void *src, size_t n)
{
	unsigned char	*d = (unsigned char *)dest;
	unsigned char	*s = (unsigned char *)src;
	if (!d || !s) return (dest);
	while (n--) *d++ = *s++;
	return (dest);
}

t_block*  get_next_physical_block(t_block* block)
{
    size_t  size;
    t_block *next;
    void    *page_end;

    if (block == NULL) return (NULL);
    
    size = get_block_size(block);
    next = addr_offset(block, SIZEOF_BLOCK + size);

    page_end = (void*)((uintptr_t)block->page + block->page->size);

    if ((void*)next < page_end) return (next);
    return (NULL);
}