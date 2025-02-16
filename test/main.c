#include <stdio.h>
#include <stddef.h>

#include "malloc.h"

void test_memory_allocator()
{
    void* ptr1 = malloc(24);
    void* ptr2 = malloc(48);
    void* ptr3 = malloc(8);

    printf("Allocated: %p, %p, %p\n", ptr1, ptr2, ptr3);

    free(ptr1);
    free(ptr2);
    free(ptr3);

    void* ptr4 = malloc(16); 
    printf("Allocated after free: %p\n", ptr4);

    void* ptr5 = malloc(40);
    printf("Allocated: %p\n", ptr5);

    free(ptr4);
    free(ptr5);

    void* ptr6 = malloc(24); 
    printf("Allocated after coalescing: %p\n", ptr6);
}

int main() {
    test_memory_allocator();
    return 0;
}
