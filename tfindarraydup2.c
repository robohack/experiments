/*
 * Find the duplicate value in an un-ordered array of integers with the values
 * 0..N,X where X is in the range 0..N.
 *
 * The reason this works regardless of overflow"
 *
 * (from: <URL:http://stackoverflow.com/a/7118161/816536>)
 *
 * We say that x is congruent to y modulo k (denoted x  ≡k  y) if x and y leave
 * the same remainder when divided by k.  This is important when working on a
 * physical machine, because when an integer overflow occurs on most hardware,
 * the resulting value is congruent to the true value modulo k, where k depends
 * on the word size.  Fortunately, the following laws hold true in modular
 * arithmetic:
 *
 * For example:
 *
 * If (x  ≡k  y) and (w ≡k  z), then ((x + w)  ≡k  (y + z))
 *
 * If (x  ≡k  y) and (w ≡k  z), then ((x * w)  ≡k  (y * z))
 *
 * This means that if you want to compute the duplicate value by finding the
 * total sum of the elements of the array and subtracting out the expected
 * total, everything will work out fine even if there is an integer overflow
 * because standard arithmetic will still produce the same values (modulo k) in
 * the hardware.
 */

#include <assert.h>
#include <err.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define MAX_N	(INT_MAX / 512)

unsigned int init_array(unsigned int *, unsigned int);
void shuffle_array(unsigned int *, unsigned int);
unsigned int rand_rng(unsigned int);
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
	unsigned int *arr;
	unsigned int i;
	unsigned int n = 0;
	unsigned int wrapc = 0;
	unsigned int dup, sum;		/* _must_ be unsigned to wrap properly! */

	/*
	 * Setup....
	 */
	srand(time_seed());
	n = rand_rng(MAX_N);
	assert(n < (UINT_MAX-1));

	n = (INT_MAX / 35);
	if (!(arr = (unsigned int *) calloc((size_t) n, sizeof(*arr)))) {
		err(1, "Unable to allocate an array of %lu integers", (unsigned long) n);
	}
	printf("Array length will be: %u\n", n);

	dup = init_array(arr, n);

	shuffle_array(arr, n);

	/*
	 * Begin...
	 */

	/* Add 0+1+2+..+(N-1) */
	for (i = sum = 0; i < n - 1; i++) {
		if (sum + i < sum) {
			wrapc++;
		}
		sum += i;
	}

	printf("Sum of 1..%u: %u (wrapped %u times)\n", n, sum, wrapc);

	/* Add up the values of all the elements of the array */
	for (i = dup = 0; i < n; i++) {
		dup += arr[i];
	}

	/* Substract the sum of 1..N-1 to find the duplicate */
	dup -= sum;

	printf("Discovered duplicate: %u\n", dup);

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

	/* 0, 1, 2 ... dup */
	for (i = 0; i <= dup; i++) {
		iarr[i] = i;
	}
	/* dup ... n-1 */
	for (; i < n; i++) {
		iarr[i] = i - 1;
	}
#if 1
	show_dup_area(iarr, n, dup);
#endif
	assert(iarr[dup] == dup);
	assert(iarr[dup+1] == dup);
	/* XXX it might have been easier to just set iarr[n-1] = dup */

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
	printf("iarr[0] = %u\n", iarr[0]);
	printf("iarr[dup-2] = %u\n", iarr[dup-2]);
	printf("iarr[dup-1] = %u\n", iarr[dup-1]);
	printf("iarr[dup] = %u\n", iarr[dup]);
	printf("iarr[dup+1] = %u\n", iarr[dup+1]);
	printf("iarr[dup+2] = %u\n", iarr[dup+2]);
	printf("iarr[n-2] = %u\n", iarr[n-2]);
	printf("iarr[n-1] = %u\n", iarr[n-1]);
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
