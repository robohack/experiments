#include <assert.h>
#include <err.h>
#include <limits.h>
#include <memory.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

/*
 * There is an array of size N containing values 1..(N-3) and two of the values
 * are repeated.
 *
 * Elements are placed randomly in the array.
 *
 * E.g. in {2, 3, 6, 1, 5, 4, 0, 3, 5} N=9, and repeated numbers are 3 and 5.
 *
 * What is the best way to find the repeated numbers?
 *
 * A sure way to find 2 or more (K) duplicates will be to allocate an array of
 * N bits, plus an array of K ints, and then make K passes over the array by
 * setting the I'th bit where I is the value found at each location 1..N in the
 * array, and also remembering each duplicate and ignoring it on subsequent
 * passes.  We can quit each pass early if we find the flag already set.
 * Pseudocode for each pass would look like this (assuming "int arr[N],
 * dups[K]"):
 *
 *	bool flags[N] = {false};
 *
 *	for (i = 0; i < N; i++) {
 *		if (arr[i] in dups[])
 *			continue;
 *		if (flags[arr[i]])
 *			return arr[i];
 *		else
 *			flags[arr[i]] = true;
 *	}
 *	return -1;
 *
 * If K is unknown then it could be as large as N/2 (i.e. where every value is
 * duplicated once), and thus we also need to allocate another array of up to
 * N/2 integers.  We will need K+1 passes -- the first pass which finds no new
 * duplicates ends the search.
 *
 * If K is unknown then keeping "dups" sorted and using a binary search to
 * check if a duplicate has already been accounted for may improve performance.
 *
 * In theory if the array is writable and smaller than INT_MAX-2 in size (and
 * of course containing only positive values) then we could avoid allocating
 * the bit array and just use the sign bits in the same array, clearing them
 * afterwards, but this then requires K+1 passes, the last being to reset the
 * sign bits if we are to leave the array as it was.
 *
 * Alternately the sign bit could be used to mark already known duplicates thus
 * avoiding the need to continually search through the known "dups".  We still
 * need an extra pass to reset the sign bits if we are to leave the array as it
 * was, and we still need an array "int dups[N/2]" in which to store the
 * answers, and a counter "k" to remember how many duplicates have been found.
 *
 * Note also that even for every possible 32-bit integer, it is possible to
 * represent each in a quite-manageable 2^24-byte (16MB) array of bits.
 * Therefore it would be possible to use this solution even for arrays
 * containing completely random integers (signed or unsigned).
 */

#if 1
# define MAX_N	(INT_MAX - 3)		/* XXX because we flag zero as INT_MIN */
#else
# if 0
#  define MAX_N	512
# else
#  define MAX_N	(INT_MAX / 512)		/* for reasonable runtimes... */
# endif
#endif

typedef unsigned long bitarr_t;

#define BYTES_PER_SLOT		sizeof(bitarr_t)

#define SLOT_FOR(bit)		((bit) / BYTES_PER_SLOT)
#define BIT_NUM_FOR(bit)	((bit) % BYTES_PER_SLOT)
#define BIT_MASK_FOR(bit)	(1U << BIT_NUM_FOR(bit))

void set_bit(bitarr_t [], unsigned int);
int get_bit(bitarr_t [], unsigned int);
int clear_bit(bitarr_t [], unsigned int);

void init_array(unsigned int *, unsigned int, unsigned int *, unsigned int *);
void shuffle_array(unsigned int *, unsigned int);
unsigned int rand_rng(unsigned int);
void show_dup_area(unsigned int *, unsigned int, unsigned int, unsigned int);

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
	signed int *arr;		/* XXX but will hold unsigned values */
	unsigned int j;
	unsigned int i;
	unsigned int n = 0;
	unsigned int known_dup1 = 0;
	unsigned int known_dup2 = 0;
	bitarr_t *flags;
	signed int *dups;
	unsigned int k = 0;
	unsigned int max_k;

	/*
	 * Setup....
	 */
	srand(time_seed());

	do {
		n = rand_rng(MAX_N);
		assert(n <= MAX_N);

		if (!(arr = (signed int *) calloc((size_t) n, sizeof(*arr)))) {
			warn("an array of %lu integers is too big", (unsigned long) n);
		}
	} while (!arr);
	printf("Array length will be: %u\n", n);

	/* XXX this is more of a logic check than a range check.... */
	assert(((n / BYTES_PER_SLOT) + 1 * sizeof(bitarr_t)) >= ((n / CHAR_BIT) + 1));

	if (!(flags = (bitarr_t *) calloc((n / BYTES_PER_SLOT) + 1, sizeof(bitarr_t)))) {
		err(1, "a bit array of %lu bytes is too big", (unsigned long) ((n / BYTES_PER_SLOT) + 1) * sizeof(bitarr_t));
	}
	max_k = ((n / 2) + 1);
	if (!(dups = (signed int *) calloc(max_k, sizeof(*dups)))) {
		err(1, "an array of %lu integers for dups is too big", (unsigned long) max_k);
	}

	printf("setting up.....\n");
	init_array((unsigned int *) arr, n, &known_dup1, &known_dup2);
	printf("done setting up.\n");

	printf("Duplicates will be: %u, %u\n", known_dup1, known_dup2);

	printf("shuffling.....\n");
	shuffle_array((unsigned int *) arr, n);
	printf("done shuffling.\n");

#ifdef TEST_MORE_DUPS
	arr[n-1] = arr[n-2] = arr[n-3];
#endif

	/*
	 * Begin...
	 */

	for (j = 0; j < max_k; j++) {
		bool found_a_dup;

		memset((void *) flags, 0, (size_t) ((n / BYTES_PER_SLOT) + 1) * sizeof(bitarr_t));
		found_a_dup = false;
		for (i = 0; i < n; i++) {
			/*
			 * XXX this is an optimization for the case where we
			 * only have positive integers to deal with.  It
			 * prevents us having to look in the bit array, which
			 * even with inlined bit lookup functions would require
			 * some arithmetic and bit-wise operations where this
			 * is just a sign-bit comparison.
			 */
			if (arr[i] < 0) {
				continue;		/* has been marked */
			}
			if (get_bit(flags, (unsigned int) arr[i])) {
				unsigned int m;

				dups[k++] = (unsigned int) arr[i];
				printf("marking arr[%u] as dup (%d)\n", i, arr[i]);
#if 1
				/*
				 * This part is optional.
				 *
				 * It may improve performance for some cases,
				 * such as if the duplicates appear more than
				 * just twice, but where the cutoff is,
				 * i.e. how many copies must appear before this
				 * improves performance instead of making it
				 * slower, requires some further research, and
				 * of course using it effectively would require
				 * knowing K beforehand.
				 */
				/*
				 * mark all other copies as known duplicates
				 *
				 * do this first to avoid multiple negations
				 */
				for (m = 0; m < n; m++) {
					if (m != i && arr[m] == arr[i]) {
						printf("marking arr[%u] as dup as well (%d)\n", m, arr[m]);
						if (arr[m] == 0) {
							arr[m] = INT_MIN;
						} else {
							arr[m] *= -1;
						}
					}
				}
#endif
				if (arr[i] == 0) {
					arr[i] = INT_MIN;	/* zero is special as 0*-1==0 */
				} else {
					arr[i] *= -1;		/* mark it */
				}
				found_a_dup = true;
				break;
			} else {
				set_bit(flags, (unsigned int) abs(arr[i]));
			}
		}
		if (!found_a_dup) {
			break;
		}
	}
	/* restore all the array value sign bits */
	for (i = 0; i < n; i++) {
		if (arr[i] == INT_MIN) {
			arr[i] = 0;			/* zero is special */
		} else if (arr[i] < 0) {
			arr[i] *= -1;			/* unmark */
		}
	}

#if 0
	/* XXX only useful for debugging if we didn't suffle above... */
	show_dup_area((unsigned int *) arr, n, known_dup1, known_dup2);
#endif

	printf("found %d duplicates:\n", k);
	for (i = 0; i < k; i++) {
		if (dups[i] >= 0) {
			printf("dups[%u]: %u\n", i, dups[i]);
		}
	}

	exit(0);
}

/*
 * Note there is no bounds checking in these functions.  The unsigned index
 * parameter will of course prevent attempts to access negative bit locations,
 * but no attempt is made to prevent access beyond the end of the bit array.
 */

void
set_bit(bitarr_t array[],
        unsigned int bit)
{
	array[SLOT_FOR(bit)] |= BIT_MASK_FOR(bit);
}

int
get_bit(bitarr_t array[],
        unsigned int bit)
{
	return((array[SLOT_FOR(bit)] & BIT_MASK_FOR(bit)) != 0);
}

int
clear_bit(bitarr_t array[],
          unsigned int bit)
{
	int	obit = get_bit(array, bit);

	array[SLOT_FOR(bit)] &= ~BIT_MASK_FOR(bit);

	return(obit);
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

void
init_array(unsigned int *iarr,
           unsigned int n,
           unsigned int *dup1p,
           unsigned int *dup2p)
{
	unsigned int i;

	*dup1p = rand_rng(n);
	do {
		*dup2p = rand_rng(n);
	} while (*dup2p < *dup1p || *dup2p == *dup1p);

	/* Prepare the array */

	/* 0, 1, 2 ... dup1 */
	for (i = 0; i <= *dup1p; i++) {
		iarr[i] = i;
	}
	/* dup1 ... dup2 */
	for (; i < *dup2p + 2; i++) {
		iarr[i] = i - 1;
	}
	/* dup2 ... n-1 */
	for (; i < n; i++) {
		iarr[i] = i - 2;
	}
#if 1
	show_dup_area(iarr, n, *dup1p, *dup2p);
#endif
	assert(iarr[*dup1p] == *dup1p);
	assert(iarr[*dup1p+1] == *dup1p);
	assert(iarr[*dup2p+1] == *dup2p);
	assert(iarr[*dup2p+2] == *dup2p); /* XXX with GCC 4.1.2 this sometimes triggers! */

	/* XXX it might have been easier to just set arr[n-2] = dup1, arr[n-1] = dup2 */
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
              unsigned int dup1,
              unsigned int dup2)
{
	printf("arr[0] = %u\n", iarr[0]);
	printf("arr[dup1-2] = %u\n", iarr[dup1-2]);
	printf("arr[dup1-1] = %u\n", iarr[dup1-1]);
	printf("arr[dup1] = %u\n", iarr[dup1]);
	printf("arr[dup1+1] = %u\n", iarr[dup1+1]);
	printf("arr[dup1+2] = %u\n", iarr[dup1+2]);
	printf("arr[dup2-2] = %u\n", iarr[dup2-2]);
	printf("arr[dup2-1] = %u\n", iarr[dup2-1]);
	printf("arr[dup2] = %u\n", iarr[dup2]);
	printf("arr[dup2+1] = %u\n", iarr[dup2+1]);
	printf("arr[dup2+2] = %u\n", iarr[dup2+2]);
	printf("arr[n-2] = %u\n", iarr[n-2]);
	printf("arr[n-1] = %u\n", iarr[n-1]);
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm -f " fn "; " (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
