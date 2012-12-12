#include <sys/cdefs.h>

#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

/*
 * ilist: a list, or stack, of integers.
 *
 * ilist_init: create a new empty list
 * ilist_free: destroy list <l>
 *
 * ilist_push: add integer value <val> at the end of <l>
 * ilist_push: pop an integer value off at the end of <l>
 *
 * ilist_print: print <l> to standard output
 */
struct ilist   *ilist_init(void);
void            ilist_free(struct ilist *);
void            ilist_push(struct ilist *, int);
int             ilist_pop(struct ilist *);
bool            ilist_is_sorted(struct ilist *);
void            ilist_print(struct ilist *);
void            ilist_print_even(struct ilist *);
void            ilist_print_odd(struct ilist *);

#define MIN_ILIST_SZ	16

struct ilist {
	int            *vals;
	unsigned int    szvals;
	unsigned int    nvals;		/* aka, frame pointer */
};

struct ilist   *
ilist_init(void)
{
	struct ilist   *l = malloc(sizeof(*l));

	assert(l != NULL);

	l->vals = NULL;
	l->szvals = 0;
	l->nvals = 0;

	return l;
}

void
ilist_free(struct ilist *l)
{
	if (l->vals)
		free(l->vals);
	free(l);
}

void
ilist_push(struct ilist *l,
	   int val)
{
	/*
	 * if we always double the size of the list we reduce the overhead to
	 * inserting new items to be O(2n)
	 *
	 * XXX we assume a VM system such that even if we malloc() a huge array
	 * of UINT_MAX values, we won't actively require that much (virtual)
	 * memory for the process unless we indeed try to fill the array with
	 * that many entries.
	 */
	if (l->nvals + 1 > l->szvals) {
		assert(l->szvals <= UINT_MAX / 2);
		l->szvals = l->szvals > 0 ? l->szvals * 2 : MIN_ILIST_SZ;
		l->vals = realloc(l->vals, sizeof(*l->vals) * l->szvals);
		assert(l->vals != NULL);
	}
	l->vals[l->nvals] = val;
	l->nvals++;
}

int
ilist_pop(struct ilist *l)
{
	int             retval = 0;

	if (l->nvals > 0) {
		retval = l->vals[--(l->nvals)];
		/* XXX for now we never shrink the array */
	}

	return retval;
}

static void
ilist_print_internal(struct ilist *l,
		     int (*testfn)(int))
{
	int             printed = 0;
	unsigned int    i;

	printf("[");
	for (i = 0; i < l->nvals; i++) {
		if ((*testfn)(l->vals[i])) {
			if (printed)
				printf(",");
			printf("%d", l->vals[i]);
			printed = 1;
		}
	}
	printf("]\n");
}

static int
test_even(int val)
{
	return val % 2 == 0;
}

static int
test_odd(int val)
{
	return val % 2 == 1;
}

static int
truep(int val __unused)
{
	return 1;
}

void
ilist_print_even(struct ilist *l)
{
	ilist_print_internal(l, test_even);
}

void
ilist_print_odd(struct ilist *l)
{
	ilist_print_internal(l, test_odd);
}

void
ilist_print(struct ilist *l)
{
	ilist_print_internal(l, truep);
}

bool
ilist_is_sorted(struct ilist *l)
{
	int             prev;
	unsigned int    i;

	if (l->nvals == 0)
		return true;

	prev = l->vals[0];
	for (i = 1; i < l->nvals; i++) {
		if (l->vals[i] < prev) {
			return false;
		}
		prev = l->vals[i];
	}
	return true;
}

int
main(void)
{
	struct ilist   *l = ilist_init();
	int             i;

	for (i = 1; i <= 10; i++) {
		ilist_push(l, i);
	}
	printf("sorted? %d\n", ilist_is_sorted(l));

	ilist_print_even(l);
	ilist_print_odd(l);
	ilist_print(l);

	printf("popped = %d\n", ilist_pop(l));
	ilist_print(l);

	ilist_free(l);

	l = ilist_init();
	printf("sorted? %d\n", ilist_is_sorted(l));
	for (i = 1; i <= 100; i++) {
		ilist_push(l, (int) (rand() / (RAND_MAX + 1.0) * INT_MAX));
	}
	ilist_print(l);
	printf("sorted? %d\n", ilist_is_sorted(l));
	ilist_free(l);

	exit(0);
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: "make tilist && ./tilist"
 * End:
 */
