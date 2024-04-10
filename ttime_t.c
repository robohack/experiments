#if (__STDC_VERSION__ - 0) >= 199901L
# include <inttypes.h>
#endif
#include <time.h>

#if 0 /* XXX need SIZEOF_XXX definitions.... */

#ifndef PRIdTIME
# if SIZEOF_TIME_T == SIZEOF_INT32
#  define PRIdTIME	PRId32
# elif SIZEOF_TIME_T == SIZEOF_INT64
#  define PRIdTIME	PRId64
# endif
#endif

#endif	/* 0 */

/*
 * IEEE Std 1003.1-2017 says:  "time_t shall be an integer type."
 */


#include <stdint.h>     /* uintmax_t */
#include <stdio.h>      /* fprintf() */
#include <stdlib.h>	/* exit() */
#include <limits.h>     /* CHAR_BIT, UINT_MAX */
#include <inttypes.h>   /* format spec PRIX64:
                         * ll/l + X on 32/64-bit arch */

/*
 * The following two routines (should) work for any integer representation in
 * either 2's or 1's complement and for any #bits per byte (CHAR_BIT might not
 * be 8).
 */

/*
 * Count the bits set in any unsigned integer type (plus any signed iff using
 * 2's complement integer representation).
 *
 * Returns the PRECISION (width - padding bits - sign bit) iff given the xxx_MAX
 * value of any integer type, signed or unsigned.
 *
 * From SEI CERT C Coding Standard:
 *
 * Rules for Developing Safe, Reliable, and Secure Systems (2016)
 */
size_t popcount(uintmax_t num);

size_t
popcount(uintmax_t num)
{
	size_t cnt = 0;

	while (num != 0) {
		if (num % 2 == 1)
			cnt++;
		num >>= 1;
	}

	return cnt;
}

#define SIGN_BIT		(1)

#ifndef MIN
# define MIN(a, b)		((a) < (b)? (a): (b))
#endif

/*
 * Get the maximum value of a time_t from it's storage width.
 *
 * On error: returns (time_t)(-1) iff time_t is longer than an intmax_t (which
 * would mean it's a floating point type and longer than an intmax_t).
 *
 * ASSERTION: time_t is a signed integer type, i.e. not (unsigned, but the bit
 * pattern of (-1) treated special).
 */
time_t get_time_t_max(void);

time_t
get_time_t_max(void)
{
	time_t t0, t1 = (time_t)(-1);
	size_t size = sizeof(time_t);
	size_t prec;
	uintmax_t max;

	if (sizeof(time_t) > sizeof(uintmax_t)) {
		return t1;
	}

	/*
	 * Get an uintmax_t with all bits set that could be in a time_t.
	 *
	 * We can not do this calculation with a time_t as long we do not know
	 * its precision (overflow could occur).
	 */
	prec = MIN(popcount(UINTMAX_MAX), CHAR_BIT * sizeof(time_t));
	max = (uintmax_t) 1 << (prec - 1);
	max = max|(max - 1);

	t0 = (time_t) max;		/* maybe truncation and/or sign change
					 * happens here */

	/* Now account for any padding bits */
	prec = popcount((uintmax_t) t0) - SIGN_BIT;
	t0 = (time_t) 1 << (prec - 1);
	t1 = t0 | (t0 - 1);

#ifdef DEBUG
	fprintf(stderr, "time_t_max\t= 0x%" PRIX64 "\n", (uint64_t) t1);
	fprintf(stderr, "sizeof(time_t)\t= %3zd bytes\n", size);
	fprintf(stderr, "bits per byte\t= %3d bit\n", CHAR_BIT);
	fprintf(stderr, "precision\t= %3zd bits\n", prec);
	fprintf(stderr, "padding\t\t= %3zd bits\n",
		CHAR_BIT*size - prec - SIGN_BIT);
#endif /* DEBUG */
    return t1;
}


int main(void);

int
main(void)
{
	volatile unsigned int bits;
	volatile unsigned int char_bits;
	time_t t1;

	char_bits = 0;
	bits = 1;
	while ((~0U & (~0U >> (bits - 1))) != 1) {
		bits++;
	}
	if (bits % sizeof(int) == 0) {
		char_bits = bits / sizeof(int);
	}
	printf("char bits\t= %d %s CHAR_BIT\n", char_bits,
	       char_bits == CHAR_BIT ? "==" : "!=");

	if (sizeof(char) != 1) {
		printf("WARNING:   sizeof(char) is not '1'!!! (%lld != %lld)\n",
		       (long long) sizeof(char), (long long) 1);
	}

	t1 = get_time_t_max();

	printf("time_t_max\t= %jd\n", (uintmax_t) t1);

	printf("time_t max yr\t= %jd\n", (uintmax_t) (t1 / (365 * 24 * 60 * 60)) + 1970);

	exit(0);
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm " fn "; " (default-value 'compile-command) " CPPFLAGS='-DDEBUG' " fn " && ./" fn))
 * End:
 */
