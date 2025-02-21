#ifndef UTILS_H
#define UTILS_H

#include "data_struct.h"

void     init_block(t_block block, size_t size, size_t page_offset);


// utils
t_page  global_pages(t_page page);

t_block get_next_block(t_block block);
t_block get_prev_block(t_block block);

t_page  get_page_from_block(t_block block);

void*   addr_offset(void *addr, size_t offset);

t_block set_block_flag(t_block block, size_t flag);
void unset_block_flag(t_block block, size_t flag);

t_block set_block_size(t_block block, size_t size);

t_block remove_block(t_block block);

// alloc / release
t_block request_new_block(size_t size);

t_block request_available_block(size_t size);
t_block coalesce(t_block block);

t_block  fragment_block(t_block block, size_t size);

void    release_block(t_block block);
void    release_page(t_page page);

void    debug_show_alloc_mem(char* msg);

#endif // UTILS_H