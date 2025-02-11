#ifndef CHUNK_H
#define CHUNK_H

#include <stddef.h>

typedef struct s_block t_block;

struct s_block {
    size_t  prev_size;        
    size_t  size;
    t_block *next;
    int     free;
};

void*       get_addr_from_chunk(t_block *chunk);
t_block*    get_chunk_from_addr(void *addr);

#endif // CHUNK_H