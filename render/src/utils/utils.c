#include "malloc.h"

// Function to coalesce adjacent free chunks
void coalesce(t_block* chunk)
{
    // Coalesce with the next chunk if it's free
    t_block* next_chunk = (t_block*)((char*)chunk + chunk->size + sizeof(t_block));
    if (next_chunk->free) {
        chunk->size += next_chunk->size + sizeof(t_block); // Increase size
        chunk->next = next_chunk->next; // Link next free chunk
    } else {
        chunk->next = NULL; // No next free chunk to coalesce
    }
}
