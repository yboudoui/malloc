#ifndef UTILS_H
#define UTILS_H

#include "data_struct.h"


// utils
void    *ft_bzero(void* addr, size_t size);
t_page*  global_pages(t_page* page);
void*   addr_offset(void *addr, size_t offset);
// t_page*  get_page_from_block(t_block* block);

t_block* get_next_block(t_block* block);
t_block* remove_block_from_bins(t_block* block);

// alloc / release
t_page*  request_page(size_t size);
t_block* request_block(size_t size);

t_block* coalesce_block(t_block* block);
t_block* fragment_block(t_block* block, size_t size);

void    release_block(t_block* block);
void    release_page(t_page* page);

#endif // UTILS_H