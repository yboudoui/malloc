#include "malloc.h"

int main(void)
{
    void* ptr1 = malloc(24);
    void* ptr2 = malloc(48);
    void* ptr3 = malloc(8);

    free(ptr2);
    free(ptr3);
    free(ptr1);

    void* ptr4 = malloc(16);
    void* ptr5 = malloc(40);
    free(ptr4);
    free(ptr5);
    
    void* ptr6 = malloc(24); 
    return 0;
}
