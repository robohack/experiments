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

#ifdef __APPLE__
typedef	int64_t		longlong_t;	/* for XDR */
typedef	uint64_t	u_longlong_t;	/* for XDR */
#endif

/*
 * The number of digits in a base-10 representation of MAXINT
 *
 * The canonical answer would be:
 *
 *	ceil(log10(MAXINT)) + 1
 *
 * XXX these are poorly named, and don't allow for the sign, but they work at
 * the pre-processor level
 */
#ifndef MAXINT_B10_DIGITS
# if (__STDC__ - 0) > 0
#  if (UINT_MAX > 0xffffffffU)
#   define MAXINT_B10_DIGITS	(20)	/* for a 64-bit int: 9,223,372,036,854,775,808 */
#  else
#   define MAXINT_B10_DIGITS	(10)	/* for a 32-bit int 2,147,483,648 */
#  endif
# else
#  define MAXINT_B10_DIGITS	(10)	/* assume a 32-bit int */
# endif
#endif

#ifndef MAXLONG_B10_DIGITS
# if (__STDC__ - 0) > 0
#  if (ULONG_MAX > 0xffffffffUL)
#   define MAXLONG_B10_DIGITS	(20)	/* for a 64-bit long: 9,223,372,036,854,775,808 */
#  else
#   define MAXLONG_B10_DIGITS	(10)	/* for a 32-bit long 2,147,483,648 */
#  endif
# else
#  define MAXLONG_B10_DIGITS	(10)	/* assume a 32-bit long */
# endif
#endif

/*
 * XXX are there any implementations with >64-bit long long?
 */
#if (__STDC__ - 0) > 0
# define MAXLLONG_B10_DIGITS	(20)	/* for a 64-bit long long: 9,223,372,036,854,775,808 */
#endif

#if (MAXINT_B10_DIGITS == MAXLONG_B10_DIGITS && UINT_MAX != ULONG_MAX)
# include "ERROR:  assumptions about U*_MAX and MAX*_B10_DIGITS are wrong!"
#endif
#if (__STDC__ - 0) > 0
# if (MAXINT_B10_DIGITS <= 5 && UINT_MAX > 0xffffU)
#  include "ERROR:  assumptions about MAXINT_B10_DIGITS are wrong! (16bit int?)"
# endif
# if (MAXINT_B10_DIGITS <= 10 && UINT_MAX > 0xffffffffU)
#  include "ERROR:  assumptions about MAXINT_B10_DIGITS are wrong! (32bit int?)"
# endif
# if (MAXLONG_B10_DIGITS <= 10 && ULONG_MAX > 0xffffffffU)
#  include "ERROR:  assumptions about MAXLONG_B10_DIGITS are wrong! (64bit long?)"
# endif
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
 */
#define __MAX_B10STRLEN_FOR_UNSIGNED_TYPE(t) \
	(((((sizeof(t) * CHAR_BIT)) * 1233) >> 12) + 1)
#endif
/*
 * for signed types we need room for the sign, except for int64_t (and larger?)
 */
#define __MAX_B10STRLEN_FOR_SIGNED_TYPE(t) \
	(__MAX_B10STRLEN_FOR_UNSIGNED_TYPE(t) + ((sizeof(t) == 8) ? 0 : 1))

/*
 * NOTE: this gives a warning (for unsigned types of int and larger) saying
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

unsigned int intlog2(uintmax_t);

unsigned int
intlog2(uintmax_t v)
{
	unsigned int b = 0;

#if 0
	if (!v)				/* not valid for zero... */
		return -1;
#endif

#define step(x)	if (v >= ((uintmax_t) 1) << x)		\
			b += x, v >>= x

	if (sizeof(uintmax_t) == 8)
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
 * So, this implementation is really ceil(log10(ceil(fabs()))), which is what
 * we want:
 */
int ceil_intlog10(uintmax_t);
int
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

	printf("intlog2(%ju) = %d, msb() = %d\n", v, intlog2(v), msb(v));

	r = ((unsigned int) msb(v) + 1) * 1233 >> 12;

	return (int) r + 1 - (v < PowersOf10[r]); /* "round" up to find ceil(r) */
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
 * another way to find integer log base 10 of an integer: The Obvious Way:
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


int main(void);

int
main()
{
	char sbuf[2];

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
	printf("floor(log10(CHAR_MAX)) = %d\n", (int) floor(log10((double) CHAR_MAX)));
	printf("floor(log10(UCHAR_MAX)) = %d\n", (int) floor(log10((double) UCHAR_MAX)));
	printf("floor(log10(SHRT_MAX)) = %d\n", (int) floor(log10((double) SHRT_MAX)));
	printf("floor(log10(USHRT_MAX)) = %d\n",(int) floor(log10((double) USHRT_MAX)));
	printf("floor(log10(INT_MAX)) = %d\n", (int) floor(log10((double) INT_MAX)));
	printf("floor(log10(UINT_MAX)) = %d\n", (int) floor(log10((double) UINT_MAX)));
	printf("floor(log10(LLONG_MAX)) = %d\n", (int) floor(log10((double) LLONG_MAX)));
	printf("floor(log10(ULLONG_MAX)) = %d\n", (int) floor(log10((double) ULLONG_MAX)));
	printf("rint(log10(CHAR_MAX)) = %d\n", (int) rint(log10((double) CHAR_MAX)));
	printf("rint(log10(UCHAR_MAX)) = %d\n", (int) rint(log10((double) UCHAR_MAX)));
	printf("rint(log10(SHRT_MAX)) = %d\n", (int) rint(log10((double) SHRT_MAX)));
	printf("rint(log10(USHRT_MAX)) = %d\n",(int) rint(log10((double) USHRT_MAX)));
	printf("rint(log10(INT_MAX)) = %d\n", (int) rint(log10((double) INT_MAX)));
	printf("rint(log10(UINT_MAX)) = %d\n", (int) rint(log10((double) UINT_MAX)));
	printf("rint(log10(LLONG_MAX)) = %d\n", (int) rint(log10((double) LLONG_MAX)));
	printf("rint(log10(ULLONG_MAX)) = %d\n", (int) rint(log10((double) ULLONG_MAX)));
	printf("ceil(log10(CHAR_MAX)) = %d\n", (int) ceil(log10((double) CHAR_MAX)));
	printf("ceil(log10(UCHAR_MAX)) = %d\n", (int) ceil(log10((double) UCHAR_MAX)));
	printf("ceil(log10(SHRT_MAX)) = %d\n", (int) ceil(log10((double) SHRT_MAX)));
	printf("ceil(log10(USHRT_MAX)) = %d\n",(int) ceil(log10((double) USHRT_MAX)));
	printf("ceil(log10(INT_MAX)) = %d\n", (int) ceil(log10((double) INT_MAX)));
	printf("ceil(log10(UINT_MAX)) = %d\n", (int) ceil(log10((double) UINT_MAX)));
	printf("ceil(log10(LLONG_MAX)) = %d\n", (int) ceil(log10((double) LLONG_MAX)));
	printf("ceil(log10(ULLONG_MAX)) = %d\n", (int) ceil(log10((double) ULLONG_MAX)));

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

	printf("MAXINT_B10_DIGITS = %d\n", MAXINT_B10_DIGITS);
	printf("MAXLONG_B10_DIGITS = %d\n", MAXLONG_B10_DIGITS);

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


	printf("__MAX_B10STRLEN_FOR_INT_TYPE(u_char) =\t\t% 2d (actual=% 2d, log10=% 2d)\n",
	       __MAX_B10STRLEN_FOR_INT_TYPE(u_char),
	       snprintf(sbuf, (size_t) 1, "%llu", (u_longlong_t) UCHAR_MAX),
	       (int) ceil(log10((double) UCHAR_MAX)));
	printf("__MAX_B10STRLEN_FOR_INT_TYPE(signed char) =\t% 2d (actual=% 2d, log10=% 2d+1)\n",
	       __MAX_B10STRLEN_FOR_INT_TYPE(signed char),
	       snprintf(sbuf, (size_t) 1, "%lld", (longlong_t) CHAR_MIN),
	       (int) ceil(log10((double) CHAR_MAX)));

	printf("__MAX_B10STRLEN_FOR_INT_TYPE(u_short) =\t\t% 2d (actual=% 2d, log10=% 2d)\n",
	       __MAX_B10STRLEN_FOR_INT_TYPE(u_short),
	       snprintf(sbuf, (size_t) 1, "%llu", (u_longlong_t) USHRT_MAX),
	       (int) ceil(log10((double) USHRT_MAX)));
	printf("__MAX_B10STRLEN_FOR_INT_TYPE(short) =\t\t% 2d (actual=% 2d, log10=% 2d+1)\n",
	       __MAX_B10STRLEN_FOR_INT_TYPE(short),
	       snprintf(sbuf, (size_t) 1, "%lld", (longlong_t) SHRT_MIN),
	       (int) ceil(log10((double) SHRT_MAX)));

	printf("__MAX_B10STRLEN_FOR_INT_TYPE(u_int) =\t\t%d (actual=%d, log10=%d)\n",
	       __MAX_B10STRLEN_FOR_INT_TYPE(u_int),
	       snprintf(sbuf, (size_t) 1, "%llu", (u_longlong_t) UINT_MAX),
	       (int) ceil(log10((double) UINT_MAX)));
	printf("__MAX_B10STRLEN_FOR_INT_TYPE(int) =\t\t%d (actual=%d, log10=%d+1)\n",
	       __MAX_B10STRLEN_FOR_INT_TYPE(int),
	       snprintf(sbuf, (size_t) 1, "%lld", (longlong_t) INT_MIN),
	       (int) ceil(log10((double) INT_MAX)));

	printf("__MAX_B10STRLEN_FOR_INT_TYPE(u_long) =\t\t%d (actual=%d, log10=%d)\n",
	       __MAX_B10STRLEN_FOR_INT_TYPE(u_long),
	       snprintf(sbuf, (size_t) 1, "%llu", (u_longlong_t) ULONG_MAX),
	       (int) ceil(log10((double) ULONG_MAX)));
	printf("__MAX_B10STRLEN_FOR_INT_TYPE(long) =\t\t%d (actual=%d, log10=%d+1)\n",
	       __MAX_B10STRLEN_FOR_INT_TYPE(long),
	       snprintf(sbuf, (size_t) 1, "%lld", (longlong_t) LONG_MIN),
	       (int) ceil(log10((double) LONG_MAX)));

	printf("__MAX_B10STRLEN_FOR_INT_TYPE(u_longlong_t) =\t%d (actual=%d, log10=%d)\n",
	       __MAX_B10STRLEN_FOR_INT_TYPE(u_longlong_t),
	       snprintf(sbuf, (size_t) 1, "%llu", (u_longlong_t) ULLONG_MAX),
	       (int) ceil(log10((double) ULLONG_MAX)));
	printf("__MAX_B10STRLEN_FOR_INT_TYPE(long long) =\t%d (actual=%d, log10=%d+1)\n",
	       __MAX_B10STRLEN_FOR_INT_TYPE(long long),
	       snprintf(sbuf, (size_t) 1, "%lld", (longlong_t) LLONG_MIN),
	       (int) ceil(log10((double) LLONG_MAX)));

#if 0
	printf("%lld\n", (longlong_t) LLONG_MIN);
	printf("%llu\n", (u_longlong_t) ULLONG_MAX);
#endif


	exit(0);
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm " fn "; " (default-value 'compile-command) " LDLIBS=-lm " fn " && ./" fn))
 * End:
 */
