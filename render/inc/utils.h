#ifndef UTILS_H
#define UTILS_H

#include "data_struct.h"

// utils
t_page  global_pages(t_page page);

t_block get_next_block(t_block block);
t_block is_block_free(t_block block);

t_page  get_page_from_block(t_block block);
size_t  get_unflaged_size(t_size size);

t_size* get_tail_metadata(t_block block);

void*   get_addr_from_block(t_block block);
t_block get_block_from_addr(void *addr);
t_block set_block_to_free(t_block block);
t_block set_block_to_not_free(t_block block);


// alloc / release
t_block request_new_block(size_t size);
t_block request_available_block(size_t size);
t_block coalesce(t_block block);
void    release_block_from_page(t_block block);

void    release_block(t_block block);
void    release_page(t_page page);


void    debug_show_alloc_mem(char* msg);

#endif // UTILS_H