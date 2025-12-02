#include "malloc.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MOVE_CURSOR_UP  "\033[1A"
#define CLEAR_LINE      "\033[2K"
#define BOLD            "\033[1m"
#define CLEAR           "\033[0m"

#define RAND_MIN_SIZE 1
#define RAND_MAX_SIZE 4096 // Covers Tiny, Small and start of Large
#define ARRAY_CAPACITY 100

void*   arr_alloc[ARRAY_CAPACITY] = {NULL};

// Helper to print using standard libc (safe for the test harness)
void    log_test(const char *function_name, char *msg, size_t i)
{
    printf("%s - %s: %zu\n", function_name, msg, i);
}

int min_max_rand(int min, int max)
{
    return (min + rand() % (max - min + 1));
}

void    full_alloc(int min, int max)
{
    size_t  i = 0;
    size_t  size;

    printf("=== Filling Allocation [%d - %d] ===\n", min, max);
    while (i < ARRAY_CAPACITY)
    {
        if (arr_alloc[i] == NULL) {
            size = min_max_rand(min, max);
            arr_alloc[i] = malloc(size);
            
            // Optional: Write something to memory to verify it works
            if (arr_alloc[i])
                memset(arr_alloc[i], 42, size);
            else
                printf("Malloc returned NULL!\n");
        }
        i++;
    }
    show_alloc_mem();
}

void    full_free(void)
{
    size_t  i = 0;

    printf("=== Full Free ===\n");
    while (i < ARRAY_CAPACITY)
    {
        if (arr_alloc[i] != NULL) {
            free(arr_alloc[i]);
            arr_alloc[i] = NULL;
        }
        i++;
    }
    show_alloc_mem();
}

void    rand_free(void)
{
    size_t  i = 0;
    size_t  idx;

    printf("=== Random Free (Half) ===\n");
    while (i < ARRAY_CAPACITY / 2)
    {
        idx = min_max_rand(0, ARRAY_CAPACITY - 1);
        if (arr_alloc[idx] != NULL) {
            free(arr_alloc[idx]);
            arr_alloc[idx] = NULL;
            i++;
        }
    }
    show_alloc_mem();
}

void    run_test_cycle(int seed)
{
    if (seed == 0) seed = time(NULL);
    srand(seed);
    printf("%sSeed: %d%s\n", BOLD, seed, CLEAR);

    // 1. Alloc Tiny/Small mixed
    full_alloc(10, 2000);
    
    // 2. Free half randomly
    rand_free();
    
    // 3. Re-alloc
    full_alloc(100, 4000); // Includes some LARGE now
    
    // 4. Clean everything
    full_free();
    
    // 5. Large alloc specific test
    printf("=== Large Alloc Test ===\n");
    void *ptr = malloc(100000);
    show_alloc_mem();
    free(ptr);
    show_alloc_mem();
}

int main(int ac, char *av[])
{
    if (ac > 1 && strcmp(av[1], "--heavy") == 0) {
        int count = (ac > 2) ? atoi(av[2]) : 100;
        while (count--) run_test_cycle(0);
    } else {
        run_test_cycle(0);
    }
    return (0);
}