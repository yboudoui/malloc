#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define WHITE   "\033[97m"
#define BOLD    "\033[1m"
#define CLEAR   "\033[0m"

#include <unistd.h>

static void put_number_base_fd(int fd, long num, char *base, int base_len)
{
    if (num >= base_len)
        put_number_base_fd(fd, num / base_len, base, base_len);
    write(fd, &base[num % base_len], 1);
}

static void put_str_fd(int fd, char *str)
{
    if (str == NULL) return;
    int index = 0;
    while (str[index])
    {
        write(fd, &str[index], 1);
        index += 1;
    }
}

void print_fd(int fd, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    while (*format)
    {
        if (*format == '%' && *(format + 1))
        {
            format++;
            if (*format == 's')
                put_str_fd(fd, va_arg(args, char *));
            else if (*format == 'd')
                put_number_base_fd(1, va_arg(args, int), "0123456789", 10);
            else if (*format == 'x')
                put_number_base_fd(1, va_arg(args, unsigned int), "0123456789abcdef", 16);
        }
        else
            write(1, format, 1);
        format++;
    }
    va_end(args);
}

static t_block*  show_block_info(int fd, t_block* block)
{
    char    *color;
    size_t  size;

    if (UNFLAG(block->size) == 0) return (NULL);
    color = (block->size & FREE) ? GREEN : RED;
    size = UNFLAG(block->size);
    print_fd(fd, "%s[%d>%d]%s ",
        color,
        size, SIZEOF_BLOCK + size,
        CLEAR);
    return (get_next_block(block));
}

static t_page*   show_page_info(int fd, t_page* page)
{
    t_block* block;
    size_t  index;

    print_fd(fd, "page addr: %x |", page);
    print_fd(fd, " space: %s%d%s |",
        GREEN  , UNFLAG(page->free.size) - SIZEOF_PAGE, CLEAR
    );
    print_fd(fd, " block count: %d\n", page->block_count);

    block = &page->free;
    index = 0;
    while (block)
    {
        block = show_block_info(fd, block);
        if (index == 8) {
            write(fd, "\n", 1);
            index = 0;
        }
        index += 1;
    }
    write(fd, "\n", 1);
    return (page->next);
}

void    show_alloc_mem(void)
{
	t_page*  page;
    int     fd;

    fd = 1;
    page = global_pages(NULL);
    if (page == NULL) {
        print_fd(1, "%s%s%s %s \n", BOLD, WHITE, "No page left", CLEAR);
        return;
    }
	while (page) page = show_page_info(fd, page);
}