#ifndef DATA_STRUCT_H
#define DATA_STRUCT_H

#include <stddef.h>
#include <stdint.h>

// 1. Alignment configuration
#define ALIGNMENT 16

// 2. Zone Limits (Required for bins.c and page.c)
#define TINY_MAX        128
#define SMALL_MAX       1024

typedef struct s_block  t_block;
typedef struct s_page   t_page;

typedef enum e_zone_type {
    TINY,
    SMALL,
    LARGE
} t_zone_type;

// Block structure with strict alignment
struct s_block {
    t_block* prev_block;
    size_t   size;
    t_page*  page;
    t_block* next;
    t_block* prev;
} __attribute__((aligned(ALIGNMENT)));

struct s_page {
    t_page*     next;
    t_page*     prev;
    size_t      size;
    size_t      block_count;
    t_zone_type type;
} __attribute__((aligned(ALIGNMENT)));

// Global Heap Structure
#define MAX_BINS 128
typedef struct s_heap {
    struct {
        t_page*  tiny;
        t_page*  small;
        t_page*  large;
    } pages;
    struct {
        t_block* tiny;
        t_block* small;
        t_block* large;
    } bins;
} t_heap;

#define SIZEOF_BLOCK    sizeof(struct s_block)
#define SIZEOF_PAGE     sizeof(struct s_page)

#define UNFLAG(size)    (size_t)(size & ~0b11)
#define FLAG(size)      (size_t)(size & 0b11)
#define FREE 1

size_t  align(size_t size);
size_t  get_block_size(t_block *block);
void    set_block_free(t_block *block);
void    set_block_not_free(t_block *block);

void    init_block(t_block *block, t_page *page, size_t size);

#endif // DATA_STRUCT_H