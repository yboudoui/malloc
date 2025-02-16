#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

#define RED     "\033[31m"
#define GREEN   "\033[32m"
static t_block  show_block_info(t_block block)
{
    char    *color;

    color = RED;
    if (is_block_free(block)) color = GREEN;
    printf("%s[%ld]\033[39m ", color, block->curr_block_size);
    return (get_next_block(block));
}

static t_page   show_page_info(t_page page)
{
    t_block block;
    size_t  index;

    printf("page -> addr: %p | used space: %ld | free space: %ld | block count: %ld\n",
        page,
        page->used_space,
        page->free_space,
        page->block_count);
    block = (t_block)&page->fake_prev_block_size;
    index = 0;
    while (block)
    {
        block = show_block_info(block);
        // printf("%p", block);
        if (index == 8) {
            printf("\n");
            index = 0;
        }
        index += 1;
    }
    printf("\n");
    return (page->next);
}

void    show_alloc_mem(void)
{
    // return;
	t_page  page;

    page = global_pages(NULL);
    if (page == NULL) return;
	while (page) page = show_page_info(page);
}