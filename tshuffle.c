#include <float.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

/* partly from https://stackoverflow.com/a/6127606/816536 */

/* compile and run with
 * cc shuffle.c -o shuffle && ./shuffle */

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
static int
rand_rng(int N)
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

int main(void);

/*
 * shuffle() - Arrange the N elements of ARRAY in random order.
 *
 * Only effective as-is using rand() if N is MUCH smaller than RAND_MAX as well
 * as the number of bits that can be passed to srand(); if this may not be the
 * case, use a better random number generator
 *
 * (for a better PRNG:  https://benpfaff.org/writings/clc/random.html).
 *
 * See https://benpfaff.org/writings/clc/shuffle.html
 *
 * and https://en.wikipedia.org/wiki/Fisher%E2%80%93Yates_shuffle#Potential_sources_of_bias
 *
 * and http://c-faq.com/lib/randrange.html
 *
 * From Wikipedia:
 *
 * To shuffle an array a of n elements (indices 0..n-1):
 *
 *	for i from n−1 downto 1 do
 *		j ← random integer such that 0 ≤ j ≤ i
 *		exchange a[j] and a[i]
 *	done
 *
 * An equivalent algorithm that shuffles an array a of n elements (indices
 * 0..n-1) from left to right:
 *
 *	for i from 0 to n-1 do		// original had (n-2) which is very wrong
 *		j ← random integer such that i ≤ j < n
 *		exchange a[i] and a[j]
 *	done
 *
 * To initialize an array a of n elements to a randomly shuffled copy of source,
 * both 0-based (source could be just 'i' or 'n--' for an odered ordinal array):
 *
 *	for i from 0 to n−1 do
 *		j ← random integer such that 0 ≤ j ≤ i
 *		if j ≠ i
 *			a[i] ← a[j]
 *		endif
 *		a[j] ← source[i]
 *	done
 */
static void
shuffle(void *array,
        size_t n,
        size_t sz)
{
	unsigned char *tmp;
	unsigned char *arr = array;	/* to ensure byte-sized arithmetic on pointer */
	size_t stride = sz * sizeof(unsigned char);

	tmp = malloc(sz);

	if (n > 1) {
		size_t i;

		for (i = (n - 1); i > 0; i--) {
			size_t j;
#if defined(USE_NON_UNIFORM_SHUFFLE)
			/* see https://stackoverflow.com/a/23312695/816536 */
			size_t rnd = (size_t) rand();
			j = rnd / (RAND_MAX / i + 1);
#elif defined(USE_MODULO_RAND)
			j = rand() % (i + 1);
#elif defined(USE_DOUBLE_RAND)
			j = drand48() * (i + 1);
#else
			j = rand_rng((int) i);
#endif

			/*
			 * if this were an array of integers we could swap with
			 * XOR:
			 *
			 * array[i] ^= array[j];
			 * array[j] ^= array[i];
			 * array[i] ^= array[j];
			 */

			memcpy(tmp, arr + j * stride, sz);
			memcpy(arr + j * stride, arr + i * stride, sz);
			memcpy(arr + i * stride, tmp, sz);
		}
	}
	free(tmp);

	return;
}

#define PRINT_ARR(count, iter, stmt)                                    \
	do {                                                            \
		size_t c = (count);                                     \
                                                                        \
		printf("[");                                            \
		for (iter = 0; iter < c; iter++) {                      \
			stmt;                                           \
			if (iter < c - 1) {                             \
				printf(",");                            \
			}                                               \
		}                                                       \
		printf("]\n");                                          \
	} while (0)

struct cmplex {
	int foo;
	double bar;
};

static void
test_shuffle_intarr(int intarr[],
                    size_t nitems)
{
	size_t i;

	PRINT_ARR(nitems, i,
	          printf("%d", intarr[i]));
	shuffle(intarr, nitems, sizeof(intarr[0]));
	PRINT_ARR(nitems, i,
	          printf("%d", intarr[i]));

	return;
}

#define NELEMS(x)	(sizeof(x) / sizeof(x[0]))

int
main()
{
	printf("RAND_MAX = %llu\n", (unsigned long long) RAND_MAX);
	printf("INT_MAX  = %llu\n", (unsigned long long) INT_MAX);
	printf("UINT_MAX = %llu\n", (unsigned long long) UINT_MAX);

	srand(time_seed());

	/* int array #1 */ {
		int intarr[] = { 1, -5, 7, 3, 20, 2, 14, 99, 64, 78, -42 };

		test_shuffle_intarr(intarr, NELEMS(intarr));
	}
	/* int array #2 */ {
		int intarr[] = { 4, 9 };

		/* xxx hmmmm... can we ever shuffle 2-item arrays to the same order??? */
		test_shuffle_intarr(intarr, NELEMS(intarr));
	}
	/* int array #3 */ {
		int intarr[] = { -2, 6, 8 };

		test_shuffle_intarr(intarr, NELEMS(intarr));
	}
	/* int array #4 */ {
		int intarr[] = { 3, -7, -2, 5 };

		test_shuffle_intarr(intarr, NELEMS(intarr));
	}
	/* int array #5 */ {
		int intarr[] = { 9, -6, 1, -7, 4 };

		test_shuffle_intarr(intarr, NELEMS(intarr));
	}

	/* struct cmplex array #1 */ {
		struct cmplex cmparr[] = {
			{ 1, 3.14 },
			{ 5, 7.12 },
			{ -4, 6.72 },
			{ 9, 8.94 },
			{ 20, 1.84 },
			{ 3, 9.22 }
		};
		size_t i;

		PRINT_ARR(NELEMS(cmparr), i,
		          printf("{%d_%.*g}", cmparr[i].foo, DBL_DIG, cmparr[i].bar));
		shuffle(cmparr, NELEMS(cmparr), sizeof(cmparr[0]));
		PRINT_ARR(NELEMS(cmparr), i,
		          printf("{%d_%.*g}", cmparr[i].foo, DBL_DIG, cmparr[i].bar));
	}
	puts("");

	exit(0);
	/* NOTREACHED */
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm " fn "; " (default-value 'compile-command) " " fn " && { ./" fn "; ./" fn "; ./" fn "; ./" fn "; ./" fn "; }"))
 * End:
 */
