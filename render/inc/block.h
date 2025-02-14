#ifndef BLOCK_H
#define BLOCK_H

#include <stddef.h>

typedef struct s_block t_block;

struct s_block {
    size_t  offset;
    size_t  size;
    t_block *next;
    int     free;
};

void*       get_addr_from_block(t_block *block);
t_block*    get_block_from_addr(void *addr);

#endif // BLOCK_H