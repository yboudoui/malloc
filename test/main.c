#include "malloc.h"

#define MOVE_CURSOR_UP  "\033[1A"
#define CLEAR_LINE      "\033[2K"

#include <stdlib.h>
#include <time.h>

#define ARRAY_CAPACITY 2048

int min_max_rand(int min, int max) {
    return (min + rand() % (max - min + 1));
}

void*   arr_alloc[ARRAY_CAPACITY] = {NULL};

void    log_test(const char *function_name, char *msg, size_t i)
{
    print_fd(1, "%s%s%s - %s: %d\n", MOVE_CURSOR_UP, CLEAR_LINE, function_name, msg, i);
}

void    full_alloc(void)
{
    size_t  i;

    i = 0;
    while (i < ARRAY_CAPACITY)
    {
        if (arr_alloc[i] == NULL) {
            arr_alloc[i] = malloc(min_max_rand(0,4096*2));
            log_test(__FUNCTION__, "malloc n", i);
        }
        i += 1;
    }
    print_fd(1, "\n");
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
    return 0;
}
//infinit loop 
//  1739953422
//  1739956690



