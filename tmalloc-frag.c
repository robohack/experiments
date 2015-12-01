#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define LSIZE	65536UL
#define SSIZE	556UL
#define NL	500
#define NS	70000


struct chunk {
	void *p;
	size_t l;
};

static int
cmp(const void *a,
    const void *b)
{
	const struct chunk *x = a, *y = b;
	return (x->p > y->p) - (y->p > x->p);
}


int main(void);

/*
 * Note:  On linux setting MALLOC_MMAP_THRESHOLD_=131072 solves the fragmentation
 */

int
main()
{
	void *bigalloc[NL];
	void *smallalloc[NS];
	struct chunk clist[NL+NS];
	unsigned long long int i;

	memset(bigalloc, 0, sizeof(bigalloc));
	memset(smallalloc, 0, sizeof(smallalloc));

	for (i = 0; i < (16*1024*1024*1024ULL)/65536; i++) {
		free(bigalloc[i % NL]);
		free(smallalloc[i % NS]);
		smallalloc[i % NS] = malloc(SSIZE);
#if 1
		bigalloc[i % NL] = malloc(LSIZE + 4096);
#else
# if defined(__linux__)
		bigalloc[i % NL] = memalign(4096, LSIZE + 4096);
# else
		posix_memalign(&bigalloc[i % NL], (size_t) getpagesize(), LSIZE + getpagesize());
# endif
#endif
	}

#if defined(__linux__)
	malloc_stats();
#endif
	for (i=0; i<NL; i++)
		clist[i].l = LSIZE, clist[i].p = bigalloc[i];
	for (i=0; i<NS; i++)
		clist[i+NL].l = SSIZE, clist[i+NL].p = smallalloc[i];
	qsort(clist, (size_t) NL+NS, sizeof(*clist), cmp);
	for (i=0; i<NL+NS; i++) {
		printf("USED: %p - %p [%zu]\n", clist[i].p,
		       (char *)clist[i].p + clist[i].l, clist[i].l);
		if (i<NL+NS-1) printf("GAP:  %p - %p [%zu]\n",
		                      (char *)clist[i].p + clist[i].l, clist[i+1].p,
		                      (char *)clist[i+1].p - (char *)clist[i].p);
	}

	exit(0);
}
