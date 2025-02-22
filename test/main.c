#include "malloc.h"

#define MOVE_CURSOR_UP  "\033[1A"
#define CLEAR_LINE      "\033[2K"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define WHITE   "\033[97m"
#define BOLD    "\033[1m"
#define CLEAR   "\033[0m"

#include <stdlib.h>
#include <string.h>
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

int    rand_test(int seed)
{
    if (seed == 0) seed = time(NULL);
    srand(seed);
    print_fd(1, "%sseed: %d%s\n\n", BOLD, seed, CLEAR);
    full_alloc();
    rand_free();
    full_alloc();
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