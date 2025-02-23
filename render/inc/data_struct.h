#ifndef DATA_STRUCT_H
#define DATA_STRUCT_H

#include <stddef.h>
#include <stdint.h>

typedef struct s_block  t_block;
typedef struct s_page   t_page;

struct s_block {
    t_block* prev_block;
    size_t  size;
    t_page*  page;
    t_block* next;
    t_block* prev;
};

struct s_page {
    t_page*          next;
    t_page*          prev;
    size_t          block_count;
    struct s_block  free;
};

#define SIZEOF_BLOCK    sizeof(struct s_block)
#define SIZEOF_PAGE     sizeof(struct s_page)

#define UNFLAG(size) (size_t)(size & ~0b11)
#define FLAG(size) (size_t)(size & 0b11)

#define FREE 1

#define ALIGNMENT 8
#define MAX_BINS    128
#define MAX_FAST_SIZE 64

#endif // DATA_STRUCT_H