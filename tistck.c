#include <sys/cdefs.h>

#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

/*
 * XXX start of istk.h
 */
/*
 * istk: a simple stack of up to UINT_MAX integers.
 *
 * could easily be modified to hold objects of any consistent/constant size by
 * passing the object size to the initialization function, storing it in a new
 * field in the header, multiplying the size of the realloc() by that value,
 * and by passing a "void *" instead of an "int" to/from each value
 * manipulating function, and copying that many uchars from/to the "void *"
 */

struct istk {
	int            *vals;	/* ptr to first item in array */
	unsigned int    szvals;	/* allocated size of vals */
	unsigned int    nvals;	/* aka, frame pointer */
};
typedef struct istk istk_t;

istk_t         *istk_init(void);
void            istk_free(istk_t *);
void            istk_push(istk_t *, int);
int             istk_pop(istk_t *);
bool            istk_is_sorted(istk_t *);
void            istk_print_head(istk_t *);
void            istk_print(istk_t *);
void            istk_print_even(istk_t *);
void            istk_print_odd(istk_t *);
/*
 * XXX end of istk.h
 */

#define MIN_ISTK_SZ	16

istk_t         *
istk_init(void)
{
	return calloc((size_t) 1, sizeof(*stk));
}

void
istk_free(istk_t * stk)
{
	if (stk->vals)
		free(stk->vals);
	free(stk);
}

void
istk_push(istk_t * stk,
	  int val)
{
	/*
	 * if we always double the size of the list we reduce the overhead to
	 * inserting new items to eventually be approximately just O(2n) (f0r
	 * each item added to the list there will be, eventually, at most 2
	 * copy operations, one copy for every item added of course, and at
	 * most one copy of every previous item already in the array during
	 * each realloc() operation)
	 *
	 * XXX we assume a VM system such that even if we malloc() a huge array
	 * of UINT_MAX values, we won't actively require that much (virtual)
	 * memory for the process unless we indeed try to fill the array with
	 * that many entries.
	 */
	if (stk->nvals + 1 > stk->szvals) {
		assert(stk->szvals <= UINT_MAX / 2);
		stk->szvals = stk->szvals > 0 ? stk->szvals * 2 : MIN_ISTK_SZ;
		stk->vals = realloc(stk->vals, sizeof(*stk->vals) * stk->szvals);
		assert(stk->vals != NULL);
	}
	stk->vals[stk->nvals] = val;
	stk->nvals++;
}

int
istk_pop(istk_t * stk)
{
	int             retval = 0;

	if (stk->nvals > 0) {
		retval = stk->vals[--(stk->nvals)];
		/* XXX for now we never shrink the array */
	}
	return retval;
}

static void
istk_print_internal(istk_t * stk,
		    bool (*testfn) (int))
{
	int             printed = 0;
	unsigned int    i;

	printf("[");
	for (i = 0; i < stk->nvals; i++) {
		if ((*testfn) (stk->vals[i])) {
			if (printed)
				printf(",");
			printf("%d", stk->vals[i]);
			printed = 1;
		}
	}
	printf("]\n");
}

static bool
test_even(int val)
{
	return val % 2 == 0;
}

static bool
test_odd(int val)
{
	return val % 2 == 1;
}

static bool
truep(int val __unused)
{
	return 1;
}

void
istk_print_even(istk_t * stk)
{
	istk_print_internal(stk, test_even);
}

void
istk_print_odd(istk_t * stk)
{
	istk_print_internal(stk, test_odd);
}

void
istk_print(istk_t * stk)
{
	istk_print_internal(stk, truep);
}

void
istk_print_head(istk_t * stk)
{
	printf("vals = 0x%p\n", stk->vals);
	printf("szvals = %u\n", stk->szvals);
	printf("nvals = %u\n", stk->nvals);
}

bool
istk_is_sorted(istk_t * stk)
{
	int             prev;
	unsigned int    i;

	if (stk->nvals == 0)
		return true;

	prev = stk->vals[0];
	for (i = 1; i < stk->nvals; i++) {
		if (stk->vals[i] < prev) {
			return false;
		}
		prev = stk->vals[i];
	}

	return true;
}

int main(void);

int
main(void)
{
	istk_t         *stk = istk_init();
	int             i;

	for (i = 1; i <= 10; i++) {
		istk_push(stk, i);
	}
	printf("sorted? %s\n", istk_is_sorted(stk) ? "true" : "false");

	istk_print_head(stk);
	fputs("even: ", stdout);
	istk_print_even(stk);
	fputs("odd: ", stdout);
	istk_print_odd(stk);
	fputs("all: ", stdout);
	istk_print(stk);

	printf("popped = %d\n", istk_pop(stk));
	fputs("popped: ", stdout);
	istk_print(stk);

	istk_free(stk);
	stk = NULL;

	putchar('\n');

	stk = istk_init();
	printf("empty is sorted? %s\n", istk_is_sorted(stk) ? "true" : "false");
	for (i = 1; i <= 100; i++) {
		istk_push(stk, (int) (rand() / (RAND_MAX + 1.0) * INT_MAX));
	}
	istk_print_head(stk);
	istk_print(stk);
	printf("random is sorted? %s\n", istk_is_sorted(stk) ? "true" : "false");
	istk_free(stk);

	exit(0);
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm -f " fn "; " (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
