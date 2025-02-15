#ifndef BINS_H
#define BINS_H

#include "block.h"

#define MAX_BINS    128
#define SMALL_BIN_COUNT 16  // Number of small bins
#define FAST_BIN_COUNT 8    // Number of fast bins

#define ALIGNMENT 8
#define MAX_FAST_SIZE 64
#define SMALL_BIN_SIZE (MAX_FAST_SIZE + (SMALL_BIN_COUNT * ALIGNMENT)) // Max size for small bins

void    release_block(t_block block);
t_block request_available_block(size_t size);

t_block coalesce(t_block block);

#endif // BINS_H