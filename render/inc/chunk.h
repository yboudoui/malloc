#ifndef CHUNK_H
#define CHUNK_H

#include <stddef.h>

typedef struct s_chunk t_chunk;

struct s_chunk {
    size_t  size;        
    t_chunk *next;
    t_chunk *prev;
    int     free;         
};

void*       get_addr_from_chunk(t_chunk *chunk);
t_chunk*    get_chunk_from_addr(void *addr);

#endif // CHUNK_H