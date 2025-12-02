#ifndef UTILS_H
#define UTILS_H

#include "data_struct.h"

// Access to singleton
t_heap* get_heap(void);

// Utils
void    *ft_bzero(void* addr, size_t size);
void*   ft_memcpy(void *dest, const void *src, size_t n);
void*   addr_offset(void *addr, size_t offset);

// Block Management
t_block* get_next_block(t_block* block);
t_block* remove_block_from_bins(t_block* block);
void     insert_block_to_bins(t_block* block);

// Alloc / Release
t_block* request_block(size_t size);
t_page*  request_page(t_zone_type type, size_t size);

t_block* coalesce_block(t_block* block);
t_block* fragment_block(t_block* block, size_t size);

void    release_block(t_block* block);
void    release_page(t_page* page);

#endif // UTILS_H