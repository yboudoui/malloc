#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <strings.h>

// Add this to malloc.c
void *calloc(size_t nmemb, size_t size)
{
    size_t  total_size;
    void    *ptr;

    // Check for overflow
    if (nmemb != 0 && size > SIZE_MAX / nmemb)
        return (NULL);

    total_size = nmemb * size;
    ptr = malloc(total_size);
    write(1, "OK", 2);
    if (ptr) {
        bzero(ptr, total_size);
    }
    return (ptr);
}

int main() {
    void *ptr;

    write(1, "--- Start ---\n", 14);

    // 1. We call calloc. 
    // If it uses your malloc, we will see "MY MALLOC CALLED".
    // If it uses system malloc, we will see nothing.
    ptr = calloc(1, 100); 

    write(1, "--- Calloc done ---\n", 20);

    // 2. We try to free it.
    // If calloc used system malloc, this will likely Segfault 
    // because your free expects a header that isn't there.
    free(ptr); 

    write(1, "--- End ---\n", 12);
    return 0;
}