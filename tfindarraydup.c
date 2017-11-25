#include <assert.h>
#include <err.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

/*
 * Find the duplicate value in an un-ordered array of integers with the values
 * "1..N,X", where X is in the range "1..N"
 *
 * NOTE: with the XOR-based algorithm the range must be 1..N, not 0..N, and all
 * values 1..N must be included with only one duplicate!
 */

#define MAX_N	(INT_MAX / 512)		/* let's be reasonable... */

unsigned int rand_rng(unsigned int);
unsigned int init_array(unsigned int *, unsigned int);
void shuffle_array(unsigned int *, unsigned int);
void show_dup_area(unsigned int *, unsigned int, unsigned int);


/*
 * Choose and return an initial random seed based on the current time.
 *
 * From https://benpfaff.org/writings/clc/random-seed.html
 * Based on code by Lawrence Kirby <fred@genesis.demon.co.uk>.
 * getpid() added by "Greg A. Woods" <woods@planix.ca>
 *
 * Usage: srand(time_seed());
 */
static unsigned int
time_seed(void)
{
	time_t timeval;			/* Current time. */
	unsigned char *ptr;		/* Type punned pointed into timeval. */
	unsigned seed;			/* Generated seed. */
	size_t i;

	timeval = time((time_t *) NULL) - (time_t) getpid();
	ptr = (unsigned char *) &timeval;

	seed = 0;
	for (i = 0; i < sizeof(timeval); i++) {
		seed = (seed * (UCHAR_MAX + 2u)) + ptr[i];
	}

	return seed;
}

int main(void);

int
main()
{
	unsigned int *uintarr;
	unsigned int i;
	unsigned int n = 0;
	unsigned int known_dup;
	unsigned int found_dup;

	/*
	 * Setup....
	 */
	srand(time_seed());
	n = rand_rng(MAX_N);
	assert(n < (UINT_MAX-1));

	if (!(uintarr = (unsigned int *) calloc((size_t) n, sizeof(*uintarr)))) {
		err(1, "Unable to allocate an array of %lu integers", (unsigned long) n);
	}
	printf("Array length will be: %u\n", n);

	known_dup = init_array(uintarr, n);
	shuffle_array(uintarr, n);
#if 0
	show_dup_area(uintarr, n, known_dup);
#endif

	/*
	 * Begin...
	 */
#if 0
	/*
	 * Walking XOR -- replace each element with the XOR of its value and
	 * the XOR of all the previous values (found in the previous location).
	 *
	 * XXX obviously this example modifies array!
	 */
	for (i = 1; i < n; i++) {
		uintarr[i] = uintarr[i - 1] ^ uintarr[i] ^ i;
	}
	printf("Duplicate value in array of %d values: %d\n", n, intarr[n - 1]);
#else
	found_dup = 0;
	for (i = 0; i < n; i++) {
		found_dup = found_dup ^ uintarr[i] ^ i;
	}
	printf("Duplicate value in array of %d values: %d\n", n, found_dup);
#endif

	exit(0);
}


#ifndef static_assert
# define static_assert(expr)						\
	typedef struct {						\
		int STATIC_ASSERT_FLD_NM_(__LINE__): ((expr) ? 1 : -1);	\
	} STATIC_ASSERT_STRUCT_NM_(__LINE__)[(expr) ? 1 : -1]

# define STATIC_ASSERT_FLD_NM_(line)	STATIC_ASSERT_FLD_NM_2_(line)
# if (__STDC__ - 0) > 0
#  define STATIC_ASSERT_FLD_NM_2_(line)	compile_time_assertion_failed_at_line_##line
# else
#  define STATIC_ASSERT_FLD_NM_2_(line)	compile_time_assertion_failed_at_line_/**/line
# endif

# define STATIC_ASSERT_STRUCT_NM_(line)	STATIC_ASSERT_STRUCT_NM_2_(line)
# if (__STDC__ - 0) > 0
#  define STATIC_ASSERT_STRUCT_NM_2_(line) compile_time_assertion_failed_at_line_##line
# else
#  define STATIC_ASSERT_STRUCT_NM_2_(line) compile_time_assertion_failed_at_line_/**/line
# endif
#endif /* !defined(static_assert) */

/*
 * rand_rng() - return a random number in the range 0..N
 *
 * - assuming N is "much less" than RAND_MAX, and also assuming N is less than
 *   UINT_MAX + 1 (the maximum entropy that can be passed to srand())
 *
 * see http://c-faq.com/lib/randrange.html
 *
 * rand() is stupid -- it should have returned unsigned int and taken a range
 * parameter up to RAND_MAX
 *
 * Shifting the range to M..N would require the equivalent of:
 *
 *	M + rand() / (RAND_MAX / (N - M + 1) + 1)
 */
unsigned int
rand_rng(unsigned int N)
{
	unsigned int x = (RAND_MAX + 1u) / N;
	unsigned long y = x * N;
	unsigned int r;

	/* xxx only needed if RAND_MAX is at or near INT_MAX */
	static_assert(sizeof(unsigned long) > sizeof(unsigned int));

	/* xxx probably only needed if N is "close" to RAND_MAX, and ((RAND_MAX + 1) % N != 0) */
	do {
		r = (unsigned int) rand();
	} while (r >= y);

	return r / x;
}

unsigned int
init_array(unsigned int *iarr,
	   unsigned int n)
{
	unsigned int i, dup;

	dup = rand_rng(n);

	printf("Duplicate will be: %u\n", dup);

	/* Prepare the array */

	/* 1, 2 ... dup */
	for (i = 0; i < dup; i++) {
		iarr[i] = i + 1;
	}
	/* dup ... n */
	for (; i < n; i++) {		/* remember i++ from end of "for" above! */
		iarr[i] = i;
	}
#if 1
	show_dup_area(iarr, n, dup);
#endif
	assert(iarr[dup-1] == dup);
	assert(iarr[dup] == dup);
	/* XXX it might have been easier to just set arr[0] = dup */

	return dup;
}

void
shuffle_array(unsigned int *iarr,
	      unsigned int n)
{
	unsigned int i, r;
	unsigned int tmp;

	for (i = 0; i < n - 1; i++) {
		r = i + rand_rng(n - i);

		/*
		 * since this is an array of integers we could swap without
		 * temporary storage using XOR:
		 *
		 * array[i] ^= array[j];
		 * array[j] ^= array[i];
		 * array[i] ^= array[j];
		 */
		tmp = iarr[i];
		iarr[i] = iarr[r];
		iarr[r] = tmp;
	}
}

void
show_dup_area(unsigned int *iarr,
	      unsigned int n,
	      unsigned int dup)
{
	printf("n = %u\n", n);
	printf("arr[0] = %u\n", iarr[0]);
	printf("arr[dup-2] = %u\n", iarr[dup-2]);
	printf("arr[dup-1] = %u\n", iarr[dup-1]);
	printf("arr[dup] = %u\n", iarr[dup]);
	printf("arr[dup+1] = %u\n", iarr[dup+1]);
	printf("arr[dup+2] = %u\n", iarr[dup+2]);
	printf("arr[n-2] = %u\n", iarr[n-2]);
	printf("arr[n-1] = %u\n", iarr[n-1]);
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm " fn "; " (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
