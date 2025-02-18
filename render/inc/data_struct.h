#ifndef DATA_STRUCT_H
#define DATA_STRUCT_H

#include <stddef.h>
#include <stdint.h>

typedef struct s_block* t_block;
typedef struct s_page*  t_page;

#define SIZEOF_BLOCK    sizeof(struct s_block)
#define SIZEOF_PAGE     sizeof(struct s_page)

#define UNFLAG(size) (size_t)(size & ~0b11)
#define FLAG(size) (size_t)(size & 0b11)

struct s_block {
    size_t  prev_block_size;
    size_t  size;
    size_t  page_offset;
    t_block next;
    t_block prev;
};

struct s_page {
    t_page          next;
    t_page          prev;
    size_t          block_count;
    size_t          used_space;
    struct s_block  free_space;
};

#define FREE 1
#define UNFREE ~1

#define ALIGNMENT 8

#define MAX_BINS    128
#define SMALL_BIN_COUNT 16  // Number of small bins
#define FAST_BIN_COUNT 8    // Number of fast bins

#define MAX_FAST_SIZE 64
#define SMALL_BIN_SIZE (MAX_FAST_SIZE + (SMALL_BIN_COUNT * ALIGNMENT)) // Max size for small bins

#endif // DATA_STRUCT_H