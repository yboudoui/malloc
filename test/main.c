#include <stdio.h>
#include <stddef.h>

#include "malloc.h"

// Simple testing function
void test_memory_allocator() {
    void* ptr1 = malloc(24); // Allocate 24 bytes
    void* ptr2 = malloc(48); // Allocate 48 bytes
    void* ptr3 = malloc(8);   // Allocate 8 bytes

    printf("Allocated: %p, %p, %p\n", ptr1, ptr2, ptr3);

    free(ptr1); // Free 24 bytes
    free(ptr2); // Free 48 bytes
    free(ptr3); // Free 8 bytes

    void* ptr4 = malloc(16); // Allocate 16 bytes
    printf("Allocated after free: %p\n", ptr4);

    // Allocating more memory to trigger coalescing
    void* ptr5 = malloc(40); // Allocate 40 bytes
    printf("Allocated: %p\n", ptr5);

    free(ptr4); // Free 16 bytes
    free(ptr5); // Free 40 bytes

    // Allocating again to see if coalescing works
    void* ptr6 = malloc(24); // Allocate 24 bytes
    printf("Allocated after coalescing: %p\n", ptr6);
}

int main() {
    test_memory_allocator();
    return 0;
}
