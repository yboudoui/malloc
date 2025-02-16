#ifndef DATA_STRUCT_H
#define DATA_STRUCT_H

#include <stddef.h>

typedef struct s_block* t_block;
typedef struct s_page*  t_page;

#define SIZEOF_BLOCK    sizeof(struct s_block)
#define SIZEOF_PAGE     sizeof(struct s_page)

struct s_block {
    size_t  prev_block_size;
    size_t  curr_block_size;
    size_t  page_offset;
    t_block next;
    t_block prev;
};

struct s_page {
    size_t  used_space;
    size_t  free_space;
    size_t  block_count;
    t_page  next;
    t_page  prev;
    size_t  fake_prev_block_size; // always 0
};

#define MAX_BINS    128
#define SMALL_BIN_COUNT 16  // Number of small bins
#define FAST_BIN_COUNT 8    // Number of fast bins

#define ALIGNMENT 8
#define MAX_FAST_SIZE 64
#define SMALL_BIN_SIZE (MAX_FAST_SIZE + (SMALL_BIN_COUNT * ALIGNMENT)) // Max size for small bins

#endif // DATA_STRUCT_H