#include "malloc.h"

#define MOVE_CURSOR_UP  "\033[1A"
#define CLEAR_LINE      "\033[2K"

#include <stdlib.h>
#include <time.h>

#define ARRAY_CAPACITY 2048

int min_max_rand(int min, int max)
{
    int rand_value;

    rand_value = rand();
    return (min + rand_value % (max - min + 1));
}

void*   arr_alloc[ARRAY_CAPACITY] = {NULL};

void    log_test(const char *function_name, char *msg, size_t i)
{
    // (void)function_name;
    // (void)msg;
    // (void)i;
    print_fd(1, "%s%s%s - %s: %d\n", MOVE_CURSOR_UP, CLEAR_LINE, function_name, msg, i);
}

void    full_alloc(void)
{
    size_t  i;
    size_t  rand_value;

    i = 0;
    while (i < ARRAY_CAPACITY)
    {
        if (arr_alloc[i] == NULL) {
            rand_value = min_max_rand(0,4096*2);
            arr_alloc[i] = malloc(rand_value);
            log_test(__FUNCTION__, "malloc n", i);
        }
        i += 1;
    }
    print_fd(1, "\n");
    // show_alloc_mem();
}

void    full_free(void)
{
    size_t  i;

    i = 0;
    while (i < ARRAY_CAPACITY)
    {
        free(arr_alloc[i]);
        log_test(__FUNCTION__, "free n", i);
        arr_alloc[i] = NULL;
        i += 1;
    }
    print_fd(1, "\n");
    // show_alloc_mem();
}

void    rand_free(void)
{
    size_t  i;
    size_t  j;

    i = 0;
    while (i < ARRAY_CAPACITY / 2)
    {
        j = min_max_rand(0, ARRAY_CAPACITY);
        if (arr_alloc[j] == NULL) continue;
        free(arr_alloc[j]);
        log_test(__FUNCTION__, "free n", i);
        arr_alloc[j] = NULL;
        i += 1;
    }
    print_fd(1, "\n");
    // show_alloc_mem();
}


int main(int ac, char *av[])
{
    int seed;

    ac -= 1;
    av += 1;
    seed = ac ? atoi(*av) : time(NULL);
    print_fd(1, "seed: %d\n\n", seed);
    srand(seed);

    full_alloc();
    rand_free();
    full_alloc();
    full_free();
    return (0);
}

// 1739990054 prev_block_size
// 1739991150 [page_offset] check fragment_block
// 1739991357 [block -> next/prev] 