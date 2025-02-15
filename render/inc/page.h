#ifndef PAGE_H
#define PAGE_H

#include <stddef.h>
#include "block.h"

typedef struct s_page* t_page;

#define SIZEOF_PAGE sizeof(struct s_page)

struct s_page {
    size_t  used_space;
    size_t  free_space;
    size_t  block_count;
    t_page  next;
    size_t  fake_prev_block_size; // always 0
};

t_block request_block(size_t size);
void    release_block(t_block block);

#endif // PAGE_H