#ifndef UTILS_H
#define UTILS_H

#include "data_struct.h"
// utils
t_page  get_page_from_block(t_block block);
size_t  get_unflaged_size(size_t size);
size_t  *get_tail_metadata(t_block block);
void    set_tail_metadata(t_block block, size_t size);
void*   get_addr_from_block(t_block block);
t_block get_block_from_addr(void *addr);
t_block set_block_to_free(t_block block);

// alloc / release
t_block request_new_block(size_t size);
t_block request_available_block(size_t size);
t_block coalesce(t_block block);
void    release_block(t_block block);
void    release_page(t_page page);

#endif // UTILS_H