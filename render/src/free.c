#include "malloc.h"

void free(void* addr)
{
    t_chunk* chunk;

    if (!addr)
        return;
    chunk = get_chunk_from_addr(addr);
    if (chunk == NULL)
        return;


    size_t size = chunk->size;

    // Mark the chunk as free
    chunk->free = 1;

    // Coalesce with adjacent free chunks
    coalesce(chunk);

    bin_append(size, chunk);
    free(chunk);
}