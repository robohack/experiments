#include <assert.h>
#include <err.h>
#include <limits.h>
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
 * The algorithm below fails for many cases.  When it doesn't fail:
 *
 * One sum contains P and Q with the same sign, the other sum - with opposite
 * signs, all other members are eliminated.
 *
 * Note:  the sums must never overflow.
 *
 * Here's the proper sum and sum
 *
 * You know that your Array contains every number from 0 to n-3 and the two
 * repeating ones (p & q). For simplicity, lets ignore the 0-case for now.
 *
 * You can calculate the sum and the product over the array, resulting in:
 *
 *	1 + 2 + ... + n-3 + p + q = p + q + (n-3)(n-2)/2
 *
 * So if you substract (n-3)(n-2)/2 from the sum of the whole array, you get
 *
 *	sum(Array) - (n-3)(n-2)/2 = x = p + q
 *
 * Now do the same for the product of the array, dividing out factorial(n-3):
 *
 *	1 * 2 * ... * n - 3 * p * q = (n - 3)! * p * q
 *
 *	prod(Array) / (n - 3)! = y = p * q
 *
 * You have now calculated these terms:
 *
 *	x = p + q
 *
 *	y = p * q
 *
 * =>	y(p + q) = x(p * q)
 *
 * If you transform these terms, you should be able to calculate p and q
 */

#define MAX_N	(INT_MAX / 512)		/* for reasonable runtimes... */

int signed_1(unsigned int);
int signed_2(unsigned int);
void init_array(unsigned int *, unsigned int, unsigned int *, unsigned int *);
void shuffle_array(unsigned int *, unsigned int);
unsigned int rand_rng(unsigned int);
void show_dup_area(unsigned int *, unsigned int, unsigned int, unsigned int);

int
signed_1(unsigned int n)
{
	return n % 2 < 1 ? +n : -n;		/* 0,-1,+2,-3,+4,-5,+6,-7,... */
}

int
signed_2(unsigned int n)
{
	return n % 4 < 2 ? +n : -n;		/* 0,+1,-2,-3,+4,+5,-6,-7,... */
}

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
	unsigned int known_dup1;
	unsigned int known_dup2;
	unsigned int found_dup1;
	unsigned int found_dup2;
	long long int sum1 = 0;
	long long int sum2 = 0;
	unsigned int wrapc1 = 0;
	unsigned int wrapc2 = 0;

	/*
	 * Setup....
	 */
	srand(time_seed());
	n = rand_rng(MAX_N);
	assert(n < (UINT_MAX-1));

	if (!(arr = (unsigned int *) calloc((size_t) n, sizeof(*arr)))) {
		err(1, "Unable to allocate an array of %lu integers", (unsigned long) n);
	}
	printf("Array length will be: %u\n", n);

	init_array(arr, n, &known_dup1, &known_dup2);

	shuffle_array(arr, n);

	/*
	 * Begin...
	 */

	for (i = 0; i < n - 2; ++i) {
		int v1 = signed_1(arr[i]) - signed_1(i);
		int v2 = signed_2(arr[i]) - signed_2(i);
		long long int t1;
		long long int t2;

		t1 = sum1 + v1;
		/* XXX note:  t1 is the sum of sum1 and v1 */
		if (((sum1 & v1 & ~t1) | (~sum1 & ~v1 & t1)) < 0) {
			printf("***  2's-complement integer addition overflow: %lld + %d = %lld!\n", sum1, v1, t1);
			wrapc1++;
		}
		sum1 = t1;

		t2 = sum2 + v2;
		/* XXX note:  t2 is the sum of sum2 and v2 */
		if (((sum2 & v2 & ~t2) | (~sum2 & ~v2 & t2)) < 0) {
			printf("***  2's-complement integer addition overflow: %lld + %d = %lld!\n", sum2, v2, t2);
			wrapc1++;
		}
		sum2 = t2;
	}

	if (wrapc1 > 0) {
		printf("Sum1 of differences: %lld (wrapped %u times)\n", sum1, wrapc1);
	}
	if (wrapc2 > 0) {
		printf("Sum2 of differences: %lld (wrapped %u times)\n", sum2, wrapc2);
	}

	for (i = n - 2; i < n; ++i) {
		int v1 = signed_1(arr[i]);
		int v2 = signed_2(arr[i]);
		long long int t1;
		long long int t2;

		t1 = sum1 + v1;
		/* XXX note:  t1 is the sum of sum1 and v1 */
		if (((sum1 & v1 & ~t1) | (~sum1 & ~v1 & t1)) < 0) {
			printf("***  2's-complement integer addition overflow: %lld + %d = %lld!\n", sum1, v1, t1);
			wrapc1++;
		}
		sum1 = t1;

		t2 = sum2 + v2;
		/* XXX note:  t2 is the sum of sum2 and v2 */
		if (((sum2 & v2 & ~t2) | (~sum2 & ~v2 & t2)) < 0) {
			printf("***  2's-complement integer addition overflow: %lld + %d = %lld!\n", sum2, v2, t2);
			wrapc1++;
		}
		sum2 = t2;
	}

	printf("Sum1: %lld", sum1);
	if (wrapc1 > 0) {
		printf("(wrapped %u times)", wrapc1);
	}
	putchar('\n');
	printf("Sum2: %lld", sum2);
	if (wrapc2 > 0) {
		printf("(wrapped %u times)", wrapc2);
	}
	putchar('\n');

	if (llabs(sum1) == llabs(sum2)) {
		/*
		 * we are about to fail, but we can still do a sanity check as
		 * this should never fail.
		 */
		assert(((known_dup1 + known_dup2) == llabs(sum1)) ||
		       abs((int) known_dup1 - (int) known_dup2) == llabs(sum1));
	}

	sum1 = llabs(sum1);
	sum2 = llabs(sum2);

	/* XXX this algorithm fails in the following case */
	if (sum1 == sum2) {
		puts("Sorry, the sums have the same magnitude thus the duplicates are unidentifiable.");
		exit(1);
	}

	found_dup1 = (sum1 + sum2) / 2;
	found_dup2 = llabs(sum1 - sum2) / 2;

	printf("Discovered duplicates: %u, %u\n", found_dup1, found_dup2);

	if (found_dup1 == known_dup1) {
		assert(found_dup2 == known_dup2);
	} else if (found_dup2 == known_dup2) {
		assert(found_dup1 == known_dup1);
	} else {
		assert(found_dup1 == known_dup2);
	}

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

	printf("Duplicates will be: %u, %u\n", *dup1p, *dup2p);

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
	assert(iarr[*dup2p+2] == *dup2p);

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
	printf("n = %u\n", n);
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
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm " fn "; " (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
