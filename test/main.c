#include "malloc.h"

int main(void)
{
    void* ptr1 = malloc(24);
    show_alloc_mem();

    void* ptr2 = malloc(48);
    show_alloc_mem();

    void* ptr3 = malloc(8);

    show_alloc_mem();
    // printf("Allocated: %p, %p, %p\n", ptr1, ptr2, ptr3);

    free(ptr1);
    show_alloc_mem();

    free(ptr2);
    show_alloc_mem();

    free(ptr3);
    show_alloc_mem();

    void* ptr4 = malloc(16); 
    // printf("Allocated after free: %p\n", ptr4);

    void* ptr5 = malloc(40);
    // printf("Allocated: %p\n", ptr5);

    free(ptr4);
    free(ptr5);

    void* ptr6 = malloc(24); 
    // printf("Allocated after coalescing: %p\n", ptr6);
    return 0;
}
