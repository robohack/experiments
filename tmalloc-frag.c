#include <stdio.h>
#include <malloc.h>

#define LSIZE (65536+4096)
#define SSIZE 556
#define NL 500
#define NS 70000

int main(int argc, char **argv)
{
        void * bigalloc[NL];
        void * smallalloc[NS];
        int bptr = 0;
        int sptr = 0;
        int i;

        memset(bigalloc, 0, sizeof(bigalloc));
        memset(smallalloc, 0, sizeof(smallalloc));

        for (i = 0; i < (16*1024*1024*1024ULL)/65536; i++) {
                free(bigalloc[i % NL]);
                free(smallalloc[i % NS]);
                smallalloc[i % NS] = malloc(SSIZE);
                bigalloc[i % NL] = memalign(4096, LSIZE);
        }

        malloc_stats();
}
