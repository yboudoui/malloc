#ifndef BLOCK_H
#define BLOCK_H

#include <stddef.h>

typedef struct s_block* t_block;

#define SIZEOF_BLOCK sizeof(struct s_block)

struct s_block {
    size_t  prev_block_size;
    size_t  curr_block_size;
    size_t  page_offset;
    t_block next;
    t_block prev;
};

size_t*  get_tail_metadata(t_block block);


void*   get_addr_from_block(t_block block);
t_block get_block_from_addr(void *addr);

t_block set_block(t_block block, size_t size, size_t page_offset);
t_block set_block_to_free(t_block block);

void    remove_block(t_block block);

#endif // BLOCK_H
