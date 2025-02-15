#ifndef BINS_H
#define BINS_H

#include "block.h"

#define MAX_BINS    128
#define SMALL_BIN_COUNT 16  // Number of small bins
#define FAST_BIN_COUNT 8    // Number of fast bins

#define ALIGNMENT 8
#define MAX_FAST_SIZE 64
#define SMALL_BIN_SIZE (MAX_FAST_SIZE + (SMALL_BIN_COUNT * ALIGNMENT)) // Max size for small bins

void    bin_append(t_block block);
t_block bin_remove(size_t size);

#endif // BINS_H