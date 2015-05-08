#include <string.h>

#define SWAP(a, b) swap_internal(&(a), &(b), sizeof *(1 ? &(a) : &(b)))
static inline void swap_internal(void *a, void *b, size_t size) {
    char tmp[size];
    memcpy(tmp, a, size);
    memmove(a, b, size);
    memcpy(b, tmp, size);
}
