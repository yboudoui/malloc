#include "malloc.h"

#include <stdlib.h>
#include <time.h>

#define ARRAY_CAPACITY 2048

int min_max_rand(int min, int max) {
    return (min + rand() % (max - min + 1));
}

void*   arr_alloc[ARRAY_CAPACITY] = {NULL};

void    full_alloc(void)
{
    size_t  i;

    i = 0;
    while (i < ARRAY_CAPACITY)
    {
        if (arr_alloc[i] == NULL)
            arr_alloc[i] = malloc(min_max_rand(0,4096*2));
        i += 1;
    }
}

void    full_free(void)
{
    size_t  i;

    i = 0;
    while (i < ARRAY_CAPACITY)
    {
        free(arr_alloc[i]);
        arr_alloc[i] = NULL;
        i += 1;
    }
}

void    rand_free(void)
{
    size_t  i;
    size_t  j;
    void    *addr;

    i = 0;
    while (i < ARRAY_CAPACITY / 2)
    {
        j = min_max_rand(0,ARRAY_CAPACITY);
        addr = arr_alloc[j];
        if (addr == NULL) continue;
        free(arr_alloc[i]);
        arr_alloc[i] = NULL;
    }
}


int main(void)
{
    srand(time(NULL));

    full_alloc();
    rand_free();
    full_alloc();
    full_free();
    return 0;
}