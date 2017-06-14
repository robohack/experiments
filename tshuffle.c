#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* compile and run with
 * cc shuffle.c -o shuffle && ./shuffle */

#define NELEMS(x)  (sizeof(x) / sizeof(x[0]))

/* arrange the N elements of ARRAY in random order.
 * Only effective if N is much smaller than RAND_MAX;
 * if this may not be the case, use a better random
 * number generator. */
static void shuffle(void *array, size_t n, size_t size) {
    char tmp[size];
    char *arr = array;
    size_t stride = size * sizeof(char);

    if (n > 1) {
        size_t i;
        for (i = 0; i < n - 1; ++i) {
            size_t rnd = (size_t) rand();
            size_t j = i + rnd / (RAND_MAX / (n - i) + 1);

            memcpy(tmp, arr + j * stride, size);
            memcpy(arr + j * stride, arr + i * stride, size);
            memcpy(arr + i * stride, tmp, size);
        }
    }
}

#define print_type(count, stmt) \
    do { \
    printf("["); \
    for (size_t i = 0; i < (count); ++i) { \
        stmt; \
    } \
    printf("]\n"); \
    } while (0)

struct cmplex {
    int foo;
    double bar;
};

int main() {
    srand(time(NULL));
    /* int array example */ {
        int intarr[] = { 1, -5, 7, 3, 20, 2 };

        print_type(NELEMS(intarr), printf("%d,", intarr[i]));
        shuffle(intarr, NELEMS(intarr), sizeof(intarr[0]));
        print_type(NELEMS(intarr), printf("%d,", intarr[i]));
    }
    /* array of struct example */ {
        struct cmplex cmparr[] = {
            { 1, 3.14 },
            { 5, 7.12 },
            { 9, 8.94 },
            { 20, 1.84 }
        };

        print_type(NELEMS(cmparr), printf("{%d %f},", cmparr[i].foo, cmparr[i].bar));
        shuffle(cmparr, NELEMS(cmparr), sizeof(cmparr[0]));
        print_type(NELEMS(cmparr), printf("{%d %f},", cmparr[i].foo, cmparr[i].bar));
    }
    return 0;
}
