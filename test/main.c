#include "malloc.h"

#define MOVE_CURSOR_UP  "\033[1A"
#define CLEAR_LINE      "\033[2K"
#define RED             "\033[31m"
#define GREEN           "\033[32m"
#define WHITE           "\033[97m"
#define BOLD            "\033[1m"
#define CLEAR           "\033[0m"

#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <unistd.h>
#define RAND_MIN_SIZE 512
#define RAND_MAX_SIZE sysconf(_SC_PAGESIZE) * 3

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
    print_fd(1, "%s%s", MOVE_CURSOR_UP, CLEAR_LINE);
    print_fd(1, "%s - %s: %d\n", function_name, msg, i);
}

void    full_alloc(int min, int max)
{
    size_t  i;
    size_t  rand_value;

    i = 0;
    print_fd(1, "[%d %d]\n\n", min, max);
    while (i < ARRAY_CAPACITY)
    {
        if (arr_alloc[i] == NULL) {
            rand_value = min_max_rand(min, max);
            arr_alloc[i] = malloc(rand_value);
            log_test(__FUNCTION__, "malloc n", i);
        }
        i += 1;
    }
    show_alloc_mem();
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
    show_alloc_mem();
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
    show_alloc_mem();
}

int    rand_test(int seed)
{
    int min;
    int max;

    if (seed == 0) seed = time(NULL);
    srand(seed);
    print_fd(1, "%sseed: %d%s\n", BOLD, seed, CLEAR);
    min = min_max_rand(0, RAND_MIN_SIZE);
    max = min_max_rand(0, RAND_MAX_SIZE) + min;
    full_alloc(min, max);
    rand_free();
    min = min_max_rand(0, RAND_MIN_SIZE);
    max = min_max_rand(0, RAND_MAX_SIZE) + min;
    full_alloc(min, max);
    full_free();
    return (0);
}

void    shift_arg(int* ac, char ***av)
{
    (*ac) -= 1;
    (*av) += 1;
}

int main(int ac, char *av[])
{
    shift_arg(&ac, &av);

    if (ac == 0) return(rand_test(0));
    if (ac == 1) return(rand_test(atoi(*av)));

    if (strcmp(*av, "--heavy") == 0) {
        shift_arg(&ac, &av);
        int count = ac ? atoi(*av) : 2048;
        while (count--) rand_test(0);
    }
    return (0);
}