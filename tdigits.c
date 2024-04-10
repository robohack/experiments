#ifndef __STDC__
# include "ERROR:  your compiler is too old for this code"
/* xxx and in fact we use lrint() so really require C99 */
#endif

#include <sys/types.h>
#include <inttypes.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#if !defined(UINT_MAX) || !defined(ULONG_MAX)
# include "ERROR:  your system is too brain damaged to support!"
#endif

/*
 * The number of digits in a base-10 representation of MAXINT
 *
 * The canonical answer would be:
 *
 *	floor(log10(MAXINT)) + 1
 *
 * or:
 *
 *	ceil(log10(MAXINT))
 *
 * XXX these are poorly named, and don't allow for the sign, but they work at
 * the pre-processor level
 */
#ifndef MAXINT_B10_DIGITS
# if (UINT_MAX > 0xffffffffU)
#  define MAXINT_B10_DIGITS	(19)	/* for a 64-bit int: 9,223,372,036,854,775,808 */
# else
#  define MAXINT_B10_DIGITS	(10)	/* assume for a 32-bit int 2,147,483,648 */
# endif
#endif

#ifndef MAXLONG_B10_DIGITS
# if (ULONG_MAX > 0xffffffffUL)
#  define MAXLONG_B10_DIGITS	(19)	/* for a 64-bit long: 9,223,372,036,854,775,808 */
# else
#  define MAXLONG_B10_DIGITS	(10)	/* assume for a 32-bit long 2,147,483,648 */
# endif
#endif

#ifndef MAXLLONG_B10_DIGITS
# if (ULONG_MAX > 0xffffffffffffffffULL)
#  define MAXLLONG_B10_DIGITS	(39)	/* for a 128-bit long long: 170,141,183,460,469,231,731,687,303,715,884,105,728 */
# else
#  define MAXLLONG_B10_DIGITS	(19)	/* for a 64-bit long long: 9,223,372,036,854,775,807 */
# endif
#endif

#if (MAXINT_B10_DIGITS == MAXLONG_B10_DIGITS && UINT_MAX != ULONG_MAX)
# include "ERROR:  assumptions about U*_MAX and MAX*_B10_DIGITS are wrong!"
#endif
#if (MAXINT_B10_DIGITS <= 5 && UINT_MAX > 0xffffU)
# include "ERROR:  assumptions about MAXINT_B10_DIGITS are wrong! (16bit int?)"
#endif
#if (MAXINT_B10_DIGITS <= 10 && UINT_MAX > 0xffffffffU)
# include "ERROR:  assumptions about MAXINT_B10_DIGITS are wrong! (32bit int?)"
#endif
#if (MAXLONG_B10_DIGITS <= 10 && ULONG_MAX > 0xffffffffU)
# include "ERROR:  assumptions about MAXLONG_B10_DIGITS are wrong! (64bit long?)"
#endif
#if (MAXLLONG_B10_DIGITS <= 20 && ULLONG_MAX > 0xffffffffffffffffULL)
# include "ERROR:  assumptions about MAXLLONG_B10_DIGITS are wrong! (128bit long?)"
#endif


/*
 * This hack works for 8-bit through 64-bit, and accounts for the sign on
 * signed types.
 *
 * XXX but it does not work for preprocessor expressions, because of the use of
 * sizeof(), so it can't be used anywere a constant integer value is required
 * in a preprocessor expression.
 *
 * First we can easily see (or show) that sizeof(int) is the log base 2 of
 * UINT_MAX divided by the number of bits represented by one unit of sizeof()
 * (8, aka `CHAR_BIT`):
 *
 *	sizeof(int) == log2(UINT_MAX) / 8
 *
 * because UINT_MAX is of course just 2 ^ (sizeof(int) * 8)) and log2(x) is
 * the inverse of 2^x.
 *
 * We can use the identity "logb(x) = log(x) / log(b)" (where log() is the
 * natural logarithm) to find logarithms to other bases.  For example, you
 * could compute the "log base 2" of "x" using:
 *
 *	log2(x) = log(x) / log(2)
 *
 * and thus:
 *
 *	log10(x) = log(x) / log(10)
 *
 * So, we can deduce that:
 *
 *	log10(v) = log2(v) / log2(10)
 *
 * Now what we want in the end is the log base 10 of UINT_MAX and since
 * log2(10) is approximately 3, and since we know from above what log2() is in
 * terms of sizeof(), then we can say that log10(UINT_MAX) is approximately:
 *
 *	log10(UINT_MAX) ~= (sizeof(int) * 8) / 3
 *
 * That's not perfect though, especially since what we really want is the
 * ceiling value, but with some minor adjustment to account for the integer
 * rounding of log2(10) to 3, we can get what we need by first adding one to
 * the log2 term, then subtracing 1 from the result for any larger-sized
 * integer, resulting in this "good-enough" expression:
 */
#if 0
#define __MAX_B10STRLEN_FOR_UNSIGNED_TYPE(t) \
	((((sizeof(t) * CHAR_BIT) + 1) / 3) - ((sizeof(t) > 2) ? 1 : 0))
#else
/*
 * Even better we can multiply our first log2() term by 1/log2(10) (multiplying
 * by the reciprocal of the divisor is the same as dividing by the divisor),
 * and doing so makes it possible to find a better integer approximation.  I
 * most recently (re?)encountered this suggestion while reading Sean Anderson's
 * bithacks:  http://graphics.stanford.edu/~seander/bithacks.html#IntegerLog10
 *
 * To do this with integer math to the best approximation possible, we need to
 * find the ideal ratio representing our reciprocal.  This can be found by
 * searching for the smallest fractional part of multiplying our desired value
 * of 1/log2(10) by successive powers of 2, within some reasonable range of
 * powers of 2, such as with the following little AWK script:
 *
 *	awk 'BEGIN {
 *		minf=1.0
 *	}
 *	END {
 *		for (i = 1; i <= 31; i++) {
 *			a = 1.0 / (log(10) / log(2)) * 2^i
 *			if (a > (2^32 / 32))
 *				break;
 *			n = int(a)
 *			f = a - (n * 1.0)
 *			if (f < minf) {
 *				minf = f
 *				minn = n
 *				bits = i
 *			}
 *			# printf("a=%f, n=%d, f=%f, i=%d\n", a, n, f, i)
 *		}
 *		printf("%d + %f / %d, bits=%d\n", minn, minf, 2^bits, bits)
 *	}' < /dev/null
 *
 *	1233 + 0.018862 / 4096, bits=12
 *
 * So we can get a good integer approximation of multiplying our log2(v) value
 * by 1/log2(10) by multiplying it by 1233 followed by a right-shift of 12
 * (2^12 is 4096 of course):
 *
 *	log10(UINT_MAX) ~= ((((sizeof(int) * 8)) * 1233) >> 12) + 1
 *
 * and, together with adding one to do the equivalent of finding the ceiling
 * value, that gets rid of the need to fiddle with odd values:
 *
 * N.B.:  This assumes int is at least 32-bits to avoid overflow.....
 */
#define __MAX_B10STRLEN_FOR_UNSIGNED_TYPE(t) \
	((((((int) sizeof(t) * CHAR_BIT)) * 1233) >> 12) + 1)
#endif
/*
 * for signed types we need room for the sign, except for int64_t (and larger?)
 */
#define __MAX_B10STRLEN_FOR_SIGNED_TYPE(t) \
	(__MAX_B10STRLEN_FOR_UNSIGNED_TYPE(t) + ((sizeof(t) == 8) ? 0 : 1))

/*
 * N.B.: this gives a warning (for unsigned types of int and larger) saying
 * "comparison of unsigned expression < 0 is always false", and of course it
 * is, but that's what we want to know (if indeed type 't' is unsigned)!
 */
#define __MAX_B10STRLEN_FOR_INT_TYPE(t)				\
	(((t) -1 < 0) ? __MAX_B10STRLEN_FOR_SIGNED_TYPE(t)	\
	              : __MAX_B10STRLEN_FOR_UNSIGNED_TYPE(t))


/*
 * find the most significant bit set in an integer
 *
 * aka the "log base 2" of an integer
 */
unsigned int msb(uintmax_t);

unsigned int
msb(uintmax_t v)
{
        unsigned int mb = 0;

	while (v >>= 1) { /* unroll for more speed... */
		mb++;
	}

        return mb;
}

/*
 * Most Significant 1 Bit (unrolled?):
 *
 * aka "log base 2"
 *
 * a "binary search" algorithm using shift and compare operations
 *
 * The "log base 2" of an integer is the same as the position of the highest bit
 * (or most significant bit) set, the MSB.
 */
u_int msb_fast(uintmax_t);

u_int
msb_fast(uintmax_t v)
{
	u_int b = 0;

#if 0
	if (!v)
		return -1;
#endif

	/*
	 * N.B.:  this gets "warning: left shift count >= width of type"; and
	 * "warning: comparison of unsigned expression >= 0 is always true"; and
	 * "warning: right shift count >= width of type"
	 *
	 * all for the highest step (normally step(64)), even though the code
	 * has clearly established that there are _more than_ 64 bits in a
	 * uintmax_t value!
	 */
#define step(x)	if (v >= ((uintmax_t) 1) << x)		\
			b += x, v >>= x

	if (sizeof(uintmax_t) > 8)
		step(64);
	if (sizeof(uintmax_t) >= 8)
		step(32);
	if (sizeof(uintmax_t) >= 4)
		step(16);
	if (sizeof(uintmax_t) >= 2)
		step(8);
	step(4);
	step(2);
	step(1);

#undef step

	return b;
}

unsigned int intlog2(uintmax_t);

unsigned int
intlog2(uintmax_t v)
{
#ifndef __has_builtin
# define __has_builtin(x) 0  /* for compatibility */
#endif

#if __has_builtin(__builtin_clz)
	return ((sizeof(uintmax_t) * CHAR_BIT) - 1) ^ __builtin_clzll(v);
#else
	return msb_fast(v);
#endif
}


/*
 * A slightly different method is suggested by Sean Anderson:
 *
 * Find integer log base 10 of an integer
 *
 * The integer log base 10 is computed by first using one of the techniques
 * above for finding the log base 2.  By the relationship log10(v) = log2(v) /
 * log2(10), we need to multiply it by 1/log2(10), which is approximately
 * 1233/4096, or 1233 followed by a right shift of 12.  Adding one is needed
 * because the IntegerLogBase2() rounds down.  Finally, since the value t is
 * only an approximation that may be off by one, the exact value is found by
 * subtracting the result of v < PowersOf10[t].
 *
 * This method takes 6 more operations than IntegerLogBase2().  It may be sped
 * up (on machines with fast memory access) by modifying the log base 2
 * table-lookup method above so that the entries hold what is computed for t
 * (that is, pre-add, -multiply, and -shift).  Doing so would require a total
 * of only 9 operations to find the log base 10, assuming 4 tables were used
 * (one for each byte of v).
 *
 * Eric Cole suggested I add a version of this on January 7, 2006.
 *
 * Quoted from:
 *
 *	http://graphics.stanford.edu/~seander/bithacks.html#IntegerLog10
 *
 * However this doesn't work right for calculating the number of digits
 * necessary to represent a given value since for that we need the ceiling
 * value, not the floor value provided by Anderson's algorithm.  (The floor
 * value is the largest value of all '9's that can be represented, not the
 * number of digits required.)
 *
 * Note log2() doesn't really "round" down -- it's using a zero-based index!
 *
 * So, this implementation is still floor(log10(ceil(fabs(N?N:1)))) + 1, which
 * is what we want:
 */
unsigned int ceil_intlog10(uintmax_t);
unsigned int
ceil_intlog10(uintmax_t v)
{
	static unsigned long long int const PowersOf10[] =
		{ 1LLU, 10LLU, 100LLU, 1000LLU, 10000LLU, 100000LLU, 1000000LLU,
		  10000000LLU, 100000000LLU, 1000000000LLU, 10000000000LLU,
		  100000000000LLU, 1000000000000LLU, 10000000000000LLU,
		  100000000000000LLU, 1000000000000000LLU, 10000000000000000LLU,
		  100000000000000000LLU, 1000000000000000000LLU,
		  10000000000000000000LLU };
	unsigned int r;

#if 0
	printf("intlog2(%ju) = %d, msb() = %d\n", v, intlog2(v), msb(v));
#endif

	r = ((unsigned int) msb(v) + 1) * 1233 >> 12;

	return r + 1 - (v < PowersOf10[r]); /* "round" up to find ceil(r) */
}

/*
 * Here's a recursive way to find the exact number of chars needed for an
 * actual given integer value, including the sign (rather than the maximum for
 * the whole type):
 */
u_int numPlaces(intmax_t);
u_int
numPlaces(intmax_t n)
{
	if (n < 0)
		return numPlaces((n == INTMAX_MIN) ? INTMAX_MAX : -n) + 1;
	if (n < 10)
		return 1;

	return 1 + numPlaces(n / 10);
}

/*
 * and for unsigned values:
 */
u_int u_numPlaces(uintmax_t);
u_int
u_numPlaces(uintmax_t u)
{
	if (u < 10)
		return 1;

	return 1 + u_numPlaces(u / 10);
}

/*
 * another way to find integer log base 10 of an integer:  The Obvious Way:
 *
 * unsigned int v; // non-zero 32-bit integer to compute the log base 10 of
 * int r;          // result goes here
 *
 * r = (v >= 1000000000) ? 9 : (v >= 100000000) ? 8 : (v >= 10000000) ? 7 :
 *     (v >= 1000000) ? 6 : (v >= 100000) ? 5 : (v >= 10000) ? 4 :
 *     (v >= 1000) ? 3 : (v >= 100) ? 2 : (v >= 10) ? 1 : 0;
 *
 * chars_for_int = r + 1;
 */


/*
 * Kendall Willets found an "economical" solution.
 *
 * https://lemire.me/blog/2021/05/28/computing-the-number-of-digits-of-an-integer-quickly/#comment-585924
 */

/*
 * Increment the upper 32 bits (log10(T) - 1) when >= T is added.
 */
#define K(T)	(((sizeof(#T) - 1) << 32) - T)

u_int digit_count(uint32_t x);

u_int
digit_count(uint32_t x)
{

	static uint64_t table[] = {
		K(0),          K(0),          K(0),		/*    8 */
		K(10),         K(10),         K(10),		/*   64 */
		K(100),        K(100),        K(100),		/*  512 */
		K(1000),       K(1000),       K(1000),		/* 4096 */
		K(10000),      K(10000),      K(10000),		/*   32K */
		K(100000),     K(100000),     K(100000),	/*  256K */
		K(1000000),    K(1000000),    K(1000000),	/* 2048K */
		K(10000000),   K(10000000),   K(10000000),	/*   16M */
		K(100000000),  K(100000000),  K(100000000),	/*  128M */
		K(1000000000), K(1000000000), K(1000000000),	/* 1024M */
		K(1000000000), K(1000000000)			/*    4B */
	};
	u_int lg2 = intlog2(x);
	uint64_t n = (uint64_t) x + table[lg2];

	return (u_int) (n >> 32);
}


/*
 * Integer log base 10, modified binary search, from StackOverflow....
 *
 * XXX seems to assume int == int32
 */
int ilog10c(uint32_t x);		/* add one to result for digit count */

int
ilog10c(uint32_t x)
{
	if (x > 99) {
		if (x < 1000000) {
			if (x < 10000) {
				return 3 + ((int)(x - 1000) >> 31);
				// return 3 - ((x - 1000) >> 31);              // Alternative.
				// return 2 + ((999 - x) >> 31);               // Alternative.
				// return 2 + ((x + 2147482648) >> 31);        // Alternative.
			} else {
				return 5 + ((int)(x - 100000) >> 31);
			}
		} else {
			if (x < 100000000) {
				return 7 + ((int)(x - 10000000) >> 31);
			} else {
				return 9 + ((int)((x-1000000000)&~x) >> 31);
				// return 8 + (((x + 1147483648) | x) >> 31);  // Alternative.
			}
		}
	}
	/* else */
	if (x > 9) {
		return 1;
	}
	/* else */
	return ((int)(x - 1) >> 31);
	// return ((int)(x - 1) >> 31) | ((unsigned)(9 - x) >> 31);  // Alternative.
	// return (x > 9) + (x > 0) - 1;                             // Alternative.
}


int main(void);

int
main()
{
	int i;
	static unsigned long long int const values[] = {
		1LLU,
		10LLU,
		100LLU,
		1000LLU,
		10000LLU,
		100000LLU,
		1000000LLU,
		10000000LLU,
		100000000LLU,
		1000000000LLU,
		10000000000LLU,
		100000000000LLU,
		1000000000000LLU,
		10000000000000LLU,
		100000000000000LLU,
		1000000000000000LLU,
		10000000000000000LLU,		/* -1 conversion to double is inaccurate */
		100000000000000000LLU,		/* -1 conversion to double is inaccurate */
		1000000000000000000LLU,		/* -1 conversion to double is inaccurate */
		10000000000000000000LLU,	/* -1 conversion to double is inaccurate */
		CHAR_MAX,
		UCHAR_MAX,
		SHRT_MAX,
		USHRT_MAX,
		INT_MAX,
		UINT_MAX,
		LONG_MAX,
		ULONG_MAX,
		LLONG_MAX,
		ULLONG_MAX,
	};
	int cw = 0;
	int rw = 0;
	int fw = 0;
	int ciw = 0;

	/*
	 * N.B.:  using an IEEE 754 floating point implementation of log10(3f),
	 * means that any integer value larger than 2^53 bits (signed, i.e. plus
	 * a sign bit) (i.e. values with more than 15 base-10 digits) may suffer
	 * rounding to a representable value and thus may result in the "wrong"
	 * log10() result....
	 *
	 * Note also that ceil(log10()) fails for exact powers of 10, so if
	 * integer modulo plus a comparison with zero is as fast as addition
	 * then this would correct it (but unless ceil() also faster than
	 * floor(), it's a somewhat pointless excercise):
	 *
	 * 	y = imaxabs(v ? v : 1);
	 * 	x = ceil(log10(y));
	 * 	if (y % 10 == 0)
	 * 		x += 1;
	 */
	for (i = 0; i < (int) (sizeof(values) / sizeof(values[0])); i++) {
/* */		if (values[i]-1 != 0) {
		printf("[%2d] % 2d: ceil(log10(%llu)) = %ld\n", i, u_numPlaces(values[i]-1), values[i]-1, lrint(ceil(log10((double) values[i]-1))));
		if (lrint(ceil(log10((double) values[i]-1))) != u_numPlaces(values[i]-1)) {
			printf("XXX: ceil() is wrong!  (log10(%llu) = %f)\n", values[i]-1, log10((double) values[i]-1));
			cw++;
		}
		printf("[%2d] % 2d: rint(log10(%llu))+1 = %ld\n", i, u_numPlaces(values[i]-1), values[i]-1, lrint(log10((double) values[i]-1)) + 1);
		if (lrint(log10((double) values[i]-1)) + 1 != u_numPlaces(values[i]-1)) {
			printf("XXX: rint() is wrong! (log10(%llu) = %f)\n", values[i]-1, log10((double) values[i]-1));
			rw++;
		}
		printf("[%2d] % 2d: floor(log10(%llu))+1 = %ld\n", i, u_numPlaces(values[i]-1), values[i]-1, lrint(floor(log10((double) values[i]-1))) + 1);
		if (lrint(floor(log10((double) values[i]-1))) + 1 != u_numPlaces(values[i]-1)) {
			printf("XXX: floor() is wrong! (log10(%llu) = %f)\n", values[i]-1, log10((double) values[i]-1));
			fw++;
		}
		printf("[%2d] % 2d: ceil_intlog10(%llu) = %d\n", i, u_numPlaces(values[i]-1), values[i]-1, ceil_intlog10(values[i]-1));
		if (ceil_intlog10(values[i]-1) != u_numPlaces(values[i]-1)) {
			printf("XXX: ceil_intlog10() is wrong! (log10(%llu) = %f)\n", values[i]-1, log10((double) values[i]-1));
			ciw++;
		}
		putchar('\n');
/* */		}

		printf("[%2d] % 2d: ceil(log10(%llu)) = %ld\n", i, u_numPlaces(values[i]), values[i], lrint(ceil(log10((double) values[i]))));
		if (lrint(ceil(log10((double) values[i]))) != u_numPlaces(values[i])) {
			printf("XXX: ceil() is wrong! (log10(%llu) = %f)\n", values[i], log10((double) values[i]));
			cw++;
		}
		printf("[%2d] % 2d: rint(log10(%llu))+1 = %ld\n", i, u_numPlaces(values[i]), values[i], lrint(log10((double) values[i])) + 1);
		if (lrint(log10((double) values[i])) + 1 != u_numPlaces(values[i])) {
			printf("XXX: rint() is wrong! (log10(%llu) = %f)\n", values[i], log10((double) values[i]));
			rw++;
		}
		printf("[%2d] % 2d: floor(log10(%llu))+1 = %ld\n", i, u_numPlaces(values[i]), values[i], lrint(floor(log10((double) values[i]))) + 1);
		if (lrint(floor(log10((double) values[i]))) + 1 != u_numPlaces(values[i])) {
			printf("XXX: floor() is wrong! (log10(%llu) = %f)\n", values[i], log10((double) values[i]));
			fw++;
		}
		printf("[%2d] % 2d: ceil_intlog10(%llu) = %d\n", i, u_numPlaces(values[i]), values[i], ceil_intlog10(values[i]));
		if (ceil_intlog10(values[i]) != u_numPlaces(values[i])) {
			printf("XXX: ceil_intlog10() is wrong! (log10(%llu) = %f)\n", values[i], log10((double) values[i]));
			ciw++;
		}
		putchar('\n');

/* */		if (values[i] < ULLONG_MAX) {
		printf("[%2d] % 2d: ceil(log10(%llu)) = %ld\n", i, u_numPlaces(values[i]+1), values[i]+1, lrint(ceil(log10((double) values[i]+1))));
		if (lrint(ceil(log10((double) values[i]+1))) != u_numPlaces(values[i]+1)) {
			printf("XXX: ceil() is wrong! (log10(%llu) = %f)\n", values[i]+1, log10((double) values[i]+1));
			cw++;
		}
		printf("[%2d] % 2d: rint(log10(%llu))+1 = %ld\n", i, u_numPlaces(values[i]+1), values[i]+1, lrint(log10((double) values[i]+1)) + 1);
		if (lrint(log10((double) values[i]+1)) + 1 != u_numPlaces(values[i]+1)) {
			printf("XXX: rint() is wrong! (log10(%llu) = %f)\n", values[i]+1, log10((double) values[i]+1));
			rw++;
		}
		printf("[%2d] % 2d: floor(log10(%llu))+1 = %ld\n", i, u_numPlaces(values[i]+1), values[i]+1, lrint(floor(log10((double) values[i]+1))) + 1);
		if (lrint(floor(log10((double) values[i]+1))) + 1 != u_numPlaces(values[i]+1)) {
			printf("XXX: floor() is wrong! (log10(%llu) = %f)\n", values[i]+1, log10((double) values[i]+1));
			fw++;
		}
		printf("[%2d] % 2d: ceil_intlog10(%llu) = %d\n", i, u_numPlaces(values[i]+1), values[i]+1, ceil_intlog10(values[i]+1));
		if (ceil_intlog10(values[i]+1) != u_numPlaces(values[i]+1)) {
			printf("XXX: ceil_intlog10() is wrong! (log10(%llu) = %f)\n", values[i]+1, log10((double) values[i]+1));
			ciw++;
		}
/* */		}
		putchar('\n');
		putchar('\n');
	}
	printf("ceil() was wrong: %d\n", cw);
	printf("rint() was wrong: %d\n", rw);
	printf("floor() was wrong: %d\n", fw);
	printf("ceil_intlog10() was wrong: %d\n", fw);

	putchar('\n');

	printf("DIGIT_SCALE=\t%20s\n", "0123456789 123456789");
	printf("CHAR_MAX=\t%20jd\n", (intmax_t) CHAR_MAX);
	printf("UCHAR_MAX=\t%20ju\n", (intmax_t) UCHAR_MAX);
	printf("SHRT_MAX=\t%20jd\n", (intmax_t) SHRT_MAX);
	printf("USHRT_MAX=\t%20ju\n", (intmax_t) USHRT_MAX);
	printf("INT_MAX=\t%20jd\n", (intmax_t) INT_MAX);
	printf("UINT_MAX=\t%20ju\n", (intmax_t) UINT_MAX);
	printf("LONG_MAX=\t%20jd\n", (intmax_t) LONG_MAX);
	printf("ULONG_MAX=\t%20ju\n", (intmax_t) ULONG_MAX);
	printf("LLONG_MAX=\t%20jd\n", (intmax_t) LLONG_MAX);
	printf("ULLONG_MAX=\t%20ju\n", (intmax_t) ULLONG_MAX);
	printf("DIGIT_SCALE=\t%20s\n", "0123456789 123456789");

	putchar('\n');

	printf("log10(CHAR_MAX) = %f\n", log10((double) CHAR_MAX));
	printf("log10(UCHAR_MAX) = %f\n",log10((double) UCHAR_MAX));
	printf("log10(SHRT_MAX) = %f\n", log10((double) SHRT_MAX));
	printf("log10(USHRT_MAX) = %f\n",log10((double) USHRT_MAX));
	printf("log10(INT_MAX) = %f\n", log10((double) INT_MAX));
	printf("log10(UINT_MAX) = %f\n",log10((double) UINT_MAX));
	printf("log10(LLONG_MAX) = %f\n", log10((double) LLONG_MAX));
	printf("log10(ULLONG_MAX) = %f\n",log10((double) ULLONG_MAX));
	printf("floor(log10(CHAR_MAX)) = %ld\n", lrint(floor(log10((double) CHAR_MAX))));
	printf("floor(log10(UCHAR_MAX)) = %ld\n", lrint(floor(log10((double) UCHAR_MAX))));
	printf("floor(log10(SHRT_MAX)) = %ld\n", lrint(floor(log10((double) SHRT_MAX))));
	printf("floor(log10(USHRT_MAX)) = %ld\n",lrint(floor(log10((double) USHRT_MAX))));
	printf("floor(log10(INT_MAX)) = %ld\n", lrint(floor(log10((double) INT_MAX))));
	printf("floor(log10(UINT_MAX)) = %ld\n", lrint(floor(log10((double) UINT_MAX))));
	printf("floor(log10(LLONG_MAX)) = %ld\n", lrint(floor(log10((double) LLONG_MAX))));
	printf("floor(log10(ULLONG_MAX)) = %ld\n", lrint(floor(log10((double) ULLONG_MAX))));
	printf("rint(log10(CHAR_MAX)) = %ld\n", lrint(rint(log10((double) CHAR_MAX))));
	printf("rint(log10(UCHAR_MAX)) = %ld\n", lrint(rint(log10((double) UCHAR_MAX))));
	printf("rint(log10(SHRT_MAX)) = %ld\n", lrint(rint(log10((double) SHRT_MAX))));
	printf("rint(log10(USHRT_MAX)) = %ld\n", lrint(rint(log10((double) USHRT_MAX))));
	printf("rint(log10(INT_MAX)) = %ld\n", lrint(rint(log10((double) INT_MAX))));
	printf("rint(log10(UINT_MAX)) = %ld\n", lrint(rint(log10((double) UINT_MAX))));
	printf("rint(log10(LLONG_MAX)) = %ld\n", lrint(rint(log10((double) LLONG_MAX))));
	printf("rint(log10(ULLONG_MAX)) = %ld\n", lrint(rint(log10((double) ULLONG_MAX))));
	printf("ceil(log10(CHAR_MAX)) = %ld\n", lrint(ceil(log10((double) CHAR_MAX))));
	printf("ceil(log10(UCHAR_MAX)) = %ld\n", lrint(ceil(log10((double) UCHAR_MAX))));
	printf("ceil(log10(SHRT_MAX)) = %ld\n", lrint(ceil(log10((double) SHRT_MAX))));
	printf("ceil(log10(USHRT_MAX)) = %ld\n", lrint(ceil(log10((double) USHRT_MAX))));
	printf("ceil(log10(INT_MAX)) = %ld\n", lrint(ceil(log10((double) INT_MAX))));
	printf("ceil(log10(UINT_MAX)) = %ld\n", lrint(ceil(log10((double) UINT_MAX))));
	printf("ceil(log10(LLONG_MAX)) = %ld\n", lrint(ceil(log10((double) LLONG_MAX))));
	printf("ceil(log10(ULLONG_MAX)) = %ld\n", lrint(ceil(log10((double) ULLONG_MAX))));

	putchar('\n');

	printf("ceil_intlog10(CHAR_MAX) = %d\n", ceil_intlog10((uintmax_t) CHAR_MAX));
	printf("ceil_intlog10(UCHAR_MAX) = %d\n", ceil_intlog10((uintmax_t) UCHAR_MAX));
	printf("ceil_intlog10(SHRT_MAX) = %d\n", ceil_intlog10((uintmax_t) SHRT_MAX));
	printf("ceil_intlog10(USHRT_MAX) = %d\n", ceil_intlog10((uintmax_t) USHRT_MAX));
	printf("ceil_intlog10(INT_MAX) = %d\n", ceil_intlog10((uintmax_t) INT_MAX));
	printf("ceil_intlog10(UINT_MAX) = %d\n", ceil_intlog10((uintmax_t) UINT_MAX));
	printf("ceil_intlog10(LONG_MAX) = %d\n", ceil_intlog10((uintmax_t) LONG_MAX));
	printf("ceil_intlog10(ULONG_MAX) = %d\n", ceil_intlog10((uintmax_t) ULONG_MAX));
	printf("ceil_intlog10(LLONG_MAX) = %d\n", ceil_intlog10((uintmax_t) LLONG_MAX));
	printf("ceil_intlog10(ULLONG_MAX) = %d\n", ceil_intlog10((uintmax_t) ULLONG_MAX));

	putchar('\n');

	printf("digit_count(CHAR_MAX) = %d\n", digit_count((uint32_t) CHAR_MAX));
	printf("digit_count(UCHAR_MAX) = %d\n", digit_count((uint32_t) UCHAR_MAX));
	printf("digit_count(SHRT_MAX) = %d\n", digit_count((uint32_t) SHRT_MAX));
	printf("digit_count(USHRT_MAX) = %d\n", digit_count((uint32_t) USHRT_MAX));
	if (sizeof(unsigned int) <= sizeof(uint32_t)) {
		printf("digit_count(INT_MAX) = %d\n", digit_count((uint32_t) INT_MAX));
		printf("digit_count(UINT_MAX) = %d\n", digit_count((uint32_t) UINT_MAX));
	}
	if (sizeof(unsigned long) <= sizeof(uint32_t)) {
		printf("digit_count(LONG_MAX) = %d\n", digit_count((uint32_t) LONG_MAX));
		printf("digit_count(ULONG_MAX) = %d\n", digit_count((uint32_t) ULONG_MAX));
	}
	if (sizeof(unsigned long long) <= sizeof(uint32_t)) {
		printf("digit_count(LLONG_MAX) = %d\n", digit_count((uint32_t) LLONG_MAX));
		printf("digit_count(ULLONG_MAX) = %d\n", digit_count((uint32_t) ULLONG_MAX));
	}
	putchar('\n');

	printf("ilog10c(CHAR_MAX) = %d\n", ilog10c((uint32_t) CHAR_MAX));
	printf("ilog10c(UCHAR_MAX) = %d\n", ilog10c((uint32_t) UCHAR_MAX));
	printf("ilog10c(SHRT_MAX) = %d\n", ilog10c((uint32_t) SHRT_MAX));
	printf("ilog10c(USHRT_MAX) = %d\n", ilog10c((uint32_t) USHRT_MAX));
	if (sizeof(unsigned int) <= sizeof(uint32_t)) {
		printf("ilog10c(INT_MAX) = %d\n", ilog10c((uint32_t) INT_MAX));
		printf("ilog10c(UINT_MAX) = %d\n", ilog10c((uint32_t) UINT_MAX));
	}
	if (sizeof(unsigned long) <= sizeof(uint32_t)) {
		printf("ilog10c(LONG_MAX) = %d\n", ilog10c((uint32_t) LONG_MAX));
		printf("ilog10c(ULONG_MAX) = %d\n", ilog10c((uint32_t) ULONG_MAX));
	}
	if (sizeof(unsigned long long) <= sizeof(uint32_t)) {
		printf("ilog10c(LLONG_MAX) = %d\n", ilog10c((uint32_t) LLONG_MAX));
		printf("ilog10c(ULLONG_MAX) = %d\n", ilog10c((uint32_t) ULLONG_MAX));
	}

	putchar('\n');

	printf("MAXINT_B10_DIGITS = %d\n", MAXINT_B10_DIGITS);
	if (u_numPlaces((uintmax_t) INT_MAX) != MAXINT_B10_DIGITS) {
		printf("XXX: MAXINT_B10_DIGITS is wrong!!! (should be: %d)\n",
		       u_numPlaces((uintmax_t) INT_MAX));
	}
	printf("MAXLONG_B10_DIGITS = %d\n", MAXLONG_B10_DIGITS);
	if (u_numPlaces((uintmax_t) LONG_MAX) != MAXLONG_B10_DIGITS) {
		printf("XXX: MAXLONG_B10_DIGITS is wrong!!! (should be: %d)\n",
		       u_numPlaces((uintmax_t) LONG_MAX));
	}
	printf("MAXLLONG_B10_DIGITS = %d\n", MAXLLONG_B10_DIGITS);
	if (u_numPlaces((uintmax_t) LLONG_MAX)  != MAXLLONG_B10_DIGITS) {
		printf("XXX: MAXLLONG_B10_DIGITS is wrong!!! (should be: %d)\n",
		       u_numPlaces((uintmax_t) LLONG_MAX));
	}

	putchar('\n');

	printf("numPlaces(CHAR_MAX) = %d\n", numPlaces((intmax_t) CHAR_MAX));
	printf("numPlaces(CHAR_MIN) = %d\n", numPlaces((intmax_t) CHAR_MIN));
	printf("u_numPlaces(UCHAR_MAX) = %d\n", u_numPlaces((uintmax_t) UCHAR_MAX));

	putchar('\n');

	printf("numPlaces(SHRT_MAX) = %d\n", numPlaces((intmax_t) SHRT_MAX));
	printf("numPlaces(SHRT_MIN) = %d\n", numPlaces((intmax_t) SHRT_MIN));
	printf("u_numPlaces(USHRT_MAX) = %d\n", u_numPlaces((uintmax_t) USHRT_MAX));

	putchar('\n');

	printf("numPlaces(INT_MAX) = %d\n", numPlaces((intmax_t) INT_MAX));
	printf("numPlaces(INT_MIN) = %d\n", numPlaces((intmax_t) INT_MIN));
	printf("u_numPlaces(UINT_MAX) = %d\n", u_numPlaces((uintmax_t) UINT_MAX));

	putchar('\n');

	printf("numPlaces(LONG_MAX) = %d\n", numPlaces((intmax_t) LONG_MAX));
	printf("numPlaces(LONG_MIN) = %d\n", numPlaces((intmax_t) LONG_MIN));
	printf("u_numPlaces(ULONG_MAX) = %d\n", u_numPlaces((uintmax_t) ULONG_MAX));

	putchar('\n');

	printf("numPlaces(LLONG_MAX) = %d\n", numPlaces((intmax_t) LLONG_MAX));
	printf("numPlaces(LLONG_MIN) = %d\n", numPlaces((intmax_t) LLONG_MIN));
	printf("u_numPlaces(ULLONG_MAX) = %d\n", u_numPlaces((uintmax_t) ULLONG_MAX));

	putchar('\n');


	/*
	 * xxx newer GCC (e.g. >9.0) uses __builtin_snprintf() here and then
	 * complains bitterly that the "output" is truncated if the size is not
	 * zero (even if/when the buffer supplied is easily large enough).
	 * Idiot compiler -- thinks it's so smart, but nope.  Duh, of course
	 * it's truncated -- we only want to find out how much space we it would
	 * use -- we know it's more than we supply.
	 *
	 * Also, apparently SUSv2 and C99 contradict each other if snprintf() is
	 * called with size==0 then SUSv2 stipulates an unspecified return value
	 * less than 1 (representing the behaviour of ancient non-standard
	 * implementations?), while C99 allows the pointer to a string to be
	 * NULL and will then return the number of characters that would have
	 * been written in case the output string pointer was valid and the size
	 * was large enough.
	 *
	 * XXX N.B.:  Note also that sometimes (e.g. on LP64 systems without
	 * native 128 ints) longlong_t (and u_longlong_t) are not actually "long
	 * long", but rather just plain "long", and again newer compilers (like
	 * GCC >9.0) will complain that %lld doesn't match longlong_t!
	 */
	printf("__MAX_B10STRLEN_FOR_INT_TYPE(u_char) =\t\t% 2d (actual=% 2d, log10=% 2ld)\n",
	       __MAX_B10STRLEN_FOR_INT_TYPE(u_char),
	       snprintf(NULL, (size_t) 0, "%llu", (unsigned long long) UCHAR_MAX),
	       lrint(ceil(log10((double) UCHAR_MAX))));
	printf("__MAX_B10STRLEN_FOR_INT_TYPE(signed char) =\t% 2d (actual=% 2d, log10=% 2ld+2)\n",
	       __MAX_B10STRLEN_FOR_INT_TYPE(signed char),
	       snprintf(NULL, (size_t) 0, "%lld", (long long) CHAR_MIN),
	       lrint(log10((double) CHAR_MAX)));

	printf("__MAX_B10STRLEN_FOR_INT_TYPE(u_short) =\t\t% 2d (actual=% 2d, log10=% 2ld)\n",
	       __MAX_B10STRLEN_FOR_INT_TYPE(u_short),
	       snprintf(NULL, (size_t) 0, "%llu", (unsigned long long) USHRT_MAX),
	       lrint(log10((double) USHRT_MAX)));
	printf("__MAX_B10STRLEN_FOR_INT_TYPE(short) =\t\t% 2d (actual=% 2d, log10=% 2ld+2)\n",
	       __MAX_B10STRLEN_FOR_INT_TYPE(short),
	       snprintf(NULL, (size_t) 0, "%lld", (long long) SHRT_MIN),
	       lrint(log10((double) SHRT_MAX)));

	printf("__MAX_B10STRLEN_FOR_INT_TYPE(u_int) =\t\t%d (actual=%d, log10=%ld)\n",
	       __MAX_B10STRLEN_FOR_INT_TYPE(u_int),
	       snprintf(NULL, (size_t) 0, "%llu", (unsigned long long) UINT_MAX),
	       lrint(log10((double) UINT_MAX)));
	printf("__MAX_B10STRLEN_FOR_INT_TYPE(int) =\t\t%d (actual=%d, log10=% 2ld+2)\n",
	       __MAX_B10STRLEN_FOR_INT_TYPE(int),
	       snprintf(NULL, (size_t) 0, "%lld", (long long) INT_MIN),
	       lrint(log10((double) INT_MAX)));

	printf("__MAX_B10STRLEN_FOR_INT_TYPE(u_long) =\t\t%d (actual=%d, log10=%ld, ceil_ilog10=%u)\n",
	       __MAX_B10STRLEN_FOR_INT_TYPE(u_long),
	       snprintf(NULL, (size_t) 0, "%llu", (unsigned long long) ULONG_MAX),
	       lrint(log10((double) ULONG_MAX)), ceil_intlog10((uintmax_t) ULONG_MAX));
	printf("__MAX_B10STRLEN_FOR_INT_TYPE(long) =\t\t%d (actual=%d, log10=%ld+2, ceil_ilog10=%u)\n",
	       __MAX_B10STRLEN_FOR_INT_TYPE(long),
	       snprintf(NULL, (size_t) 0, "%lld", (long long) LONG_MIN),
	       lrint(log10((double) LONG_MAX)), ceil_intlog10((uintmax_t) LONG_MIN));

	printf("__MAX_B10STRLEN_FOR_INT_TYPE(unsigned long long) =\t%d (actual=%d, log10=%ld, ceil_ilog10=%u)\n",
	       __MAX_B10STRLEN_FOR_INT_TYPE(unsigned long long),
	       snprintf(NULL, (size_t) 0, "%llu", (unsigned long long) ULLONG_MAX),
	       lrint(log10((double) ULLONG_MAX)), ceil_intlog10((uintmax_t) ULLONG_MAX));
	printf("__MAX_B10STRLEN_FOR_INT_TYPE(long long) =\t%d (actual=%d, log10=%ld+2, ceil_ilog10=%u)\n",
	       __MAX_B10STRLEN_FOR_INT_TYPE(long long),
	       snprintf(NULL, (size_t) 0, "%lld", (long long) LLONG_MIN),
	       lrint((log10((double) LLONG_MAX))), ceil_intlog10((uintmax_t) LLONG_MIN));

#if 0
	printf("%lld\n", (long long) LLONG_MIN);
	printf("%llu\n", (unsigned long long) ULLONG_MAX);
#endif


	exit(0);
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm " fn "; " (default-value 'compile-command) " LDLIBS=-lm " fn " && ./" fn))
 * End:
 */
