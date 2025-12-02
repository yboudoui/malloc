#ifndef DATA_STRUCT_H
#define DATA_STRUCT_H

#include <stddef.h>
#include <stdint.h>

typedef struct s_block  t_block;
typedef struct s_page   t_page;

typedef enum e_zone_type {
    TINY,
    SMALL,
    LARGE
} t_zone_type;

struct s_block {
    t_block* prev_block; // Physical previous block
    size_t   size;       // Size | Flag
    t_page*  page;
    t_block* next;       // Logical next (in free bins)
    t_block* prev;       // Logical prev (in free bins)
};

struct s_page {
    t_page*          next;
    t_page*          prev;
    size_t           block_count;
    t_zone_type      type;
    struct s_block   free_list_head; // Not used as list, but as the first block addr
};

// Global state structure (Allowed: 1 global for allocs)
#define MAX_BINS 128
typedef struct s_heap {
    t_page*  pages[3]; // 0: TINY, 1: SMALL, 2: LARGE
    t_block* bins[MAX_BINS];
} t_heap;

#define SIZEOF_BLOCK    sizeof(struct s_block)
#define SIZEOF_PAGE     sizeof(struct s_page)

#define UNFLAG(size)    (size_t)(size & ~0b11)
#define FLAG(size)      (size_t)(size & 0b11)

#define FREE 1

// 16 bytes alignment is standard for x86_64 (long double / vector instructions)
#define ALIGNMENT       16

// Subject Limits
#define TINY_MAX        128
#define SMALL_MAX       1024
// Zone sizes must hold at least 100 allocations
// We calculate this dynamically in page.c, but logic is based on these maxes

#endif // DATA_STRUCT_H