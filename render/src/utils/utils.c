#include "malloc.h"

// Function to align size
size_t align(size_t size)
{
    return (size + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1);
}

// Function to coalesce adjacent free chunks
void coalesce(t_chunk* chunk)
{
    // Coalesce with the next chunk if it's free
    t_chunk* next_chunk = (t_chunk*)((char*)chunk + chunk->size + sizeof(t_chunk));
    if (next_chunk->free) {
        chunk->size += next_chunk->size + sizeof(t_chunk); // Increase size
        chunk->next = next_chunk->next; // Link next free chunk
    } else {
        chunk->next = NULL; // No next free chunk to coalesce
    }
}
