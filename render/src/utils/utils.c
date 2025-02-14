#include "malloc.h"

// Function to coalesce adjacent free blocks
void coalesce(t_block* block)
{
    // Coalesce with the next block if it's free
    t_block* next_block = (t_block*)((char*)block + block->size + sizeof(t_block));
    if (next_block->free) {
        block->size += next_block->size + sizeof(t_block); // Increase size
        block->next = next_block->next; // Link next free block
    } else {
        block->next = NULL; // No next free block to coalesce
    }
}
