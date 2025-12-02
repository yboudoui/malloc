#include "utils.h"

void*   addr_offset(void *addr, size_t offset)
{
    return (addr ? (uint8_t*)addr + offset : NULL);
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

t_block*  get_next_block(t_block* block)
{
    size_t  size;
    if (block == NULL) return (NULL);
    size = UNFLAG(block->size);
    return (addr_offset(block, SIZEOF_BLOCK + size));
}