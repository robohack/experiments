#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

/* XXX see also tilog2.c */

/* note: originals of these macros in tdigits.c */
/*
 * this hack works for 8-bit through 64-bit, and accounts for the sign
 *
 * XXX but it only works at runtime because of the use of sizeof()
 */
#define __MAX_DIGITS_FOR_UNSIGNED_TYPE(t) \
	((((sizeof(t) * CHAR_BIT) + 1) / 3) - ((sizeof(t) > 2) ? 1 : 0))

/*
 * need room for the sign, except for int64_t (and larger?)
 */
#define __MAX_DIGITS_FOR_SIGNED_TYPE(t) \
	(__MAX_DIGITS_FOR_UNSIGNED_TYPE(t) + ((sizeof(t) == 8) ? 0 : 1))

/*
 * NOTE: this gives a warning (for int and larger) saying "comparison of
 * unsigned expression < 0 is always false", and of course it is, but that's
 * what we want to know (if indeed type 't' is unsigned)!
 */
#define __MAX_STRLEN_FOR_INT_TYPE(t)				\
	(((t) -1 < 0) ? __MAX_DIGITS_FOR_SIGNED_TYPE(t)		\
		      : __MAX_DIGITS_FOR_UNSIGNED_TYPE(t))

/*
 * see: <URL:http://graphics.stanford.edu/~seander/bithacks.html#IntegerLogObvious>
 */

/*
 * find the most significant bit set in an integer
 *
 * aka the "log base 2" of an integer
 */
unsigned int msb_twiddle(unsigned long);

unsigned int
msb_twiddle(unsigned long v)
{
        unsigned int mb = 0;

	while (v >>= 1) { /* unroll for more speed... */
		mb++;
	}

        return mb;
}

unsigned int msb(uintmax_t);

unsigned int
msb(uintmax_t v)
{
	unsigned int b = 0;

#define step(x)	if (v >= ((uintmax_t) 1) << x)			\
			b += x, v >>= x

#ifdef static_assert			/* XXX must always be a macro in C11 */
	static_assert(sizeof(uintmax_t) <= 8);
#endif
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

unsigned int msb_OlgN_fbr(uint32_t);

unsigned int
msb_OlgN_fbr(uint32_t v)
{
	const unsigned int b[] = {0x2, 0xC, 0xF0, 0xFF00, 0xFFFF0000};
	const unsigned int S[] = {1, 2, 4, 8, 16};
	int i;
	register unsigned int r = 0;

	assert(sizeof(v) * CHAR_BIT == 32);

	for (i = 4; i >= 0; i--) { // unroll for speed...
		if (v & b[i]) {
			v >>= S[i];
			r |= S[i];
		}
	}

	return r;
}

unsigned int msb_OlgN_sbr(uint32_t);

unsigned int
msb_OlgN_sbr(uint32_t v)
{
	register unsigned int r;
	register unsigned int shift;

	assert(sizeof(v) * CHAR_BIT == 32);

	r = (v > 0xFFFF) << 4U;
	v >>= r;
	shift = (v > 0xFF) << 3U;
	v >>= shift;
	r |= shift;
	shift = (v > 0xF) << 2U;
	v >>= shift;
	r |= shift;
	shift = (v > 0x3) << 1U;
	v >>= shift;
	r |= shift;
	r |= (v >> 1U);

	return r;
}

int main(void);

int
main()
{
	size_t i;

	for (i = 1; i < (sizeof(unsigned long) * CHAR_BIT); i++) {
		uint32_t v = 1 << i;

		printf("%-*lu (v/2+1)   = %d, %d, %d, %d\n", __MAX_STRLEN_FOR_INT_TYPE(typeof(v)), (unsigned long) v/2+1,
		       msb_twiddle(v/2+1),
		       msb((uintmax_t) v/2+1),
		       msb_OlgN_fbr(v/2+1),
		       msb_OlgN_sbr(v/2+1));
		if (msb_twiddle(v/2+1) != msb((uintmax_t) v/2+1))
			printf("*** msb((uintmax_t) %lu) is broken!\n", (unsigned long) v/2+1);
		if (msb_twiddle(v/2+1) != msb_OlgN_fbr(v/2+1))
			printf("*** msb_OlgN_fbr(%lu) is broken!\n", (unsigned long) v/2+1);
		if (msb_twiddle(v/2+1) != msb_OlgN_sbr(v/2+1))
			printf("*** msb_OlgN_sbr(%lu) is broken!\n", (unsigned long) v/2+1);
		printf("%-*lu (v-2)     = %d, %d, %d, %d\n", __MAX_STRLEN_FOR_INT_TYPE(typeof(v)), (unsigned long) v-2,
		       msb_twiddle(v-2),
		       msb((uintmax_t) v-2),
		       msb_OlgN_fbr(v-2),
		       msb_OlgN_sbr(v-2));
		if (msb_twiddle(v-2) != msb((uintmax_t) v-2))
			printf("*** msb((uintmax_t) %lu) is broken!\n", (unsigned long) v-2);
		if (msb_twiddle(v-2) != msb_OlgN_fbr(v-2))
			printf("*** msb_OlgN_fbr(%lu) is broken!\n", (unsigned long) v-2);
		if (msb_twiddle(v-2) != msb_OlgN_sbr(v-2))
			printf("*** msb_OlgN_sbr(%lu) is broken!\n", (unsigned long) v-2);
		printf("%-*lu (v-1)     = %d, %d, %d, %d\n", __MAX_STRLEN_FOR_INT_TYPE(typeof(v)), (unsigned long) v-1,
		       msb_twiddle(v-1),
		       msb((uintmax_t) v-1),
		       msb_OlgN_fbr(v-1),
		       msb_OlgN_sbr(v-1));
		if (msb_twiddle(v-1) != msb((uintmax_t) v-1))
			printf("*** msb((uintmax_t) %lu) is broken!\n", (unsigned long) v-1);
		if (msb_twiddle(v-1) != msb_OlgN_fbr(v-1))
			printf("*** msb_OlgN_fbr(%lu) is broken!\n", (unsigned long) v-1);
		if (msb_twiddle(v-1) != msb_OlgN_sbr(v-1))
			printf("*** msb_OlgN_sbr(%lu) is broken!\n", (unsigned long) v-1);
		printf("%-*lu (%2d bits) = %d, %d, %d, %d\n", __MAX_STRLEN_FOR_INT_TYPE(typeof(v)), (unsigned long) v, i,
		       msb_twiddle(v),
		       msb((uintmax_t) v),
		       msb_OlgN_fbr(v),
		       msb_OlgN_sbr(v));
		if (msb_twiddle(v) != msb((uintmax_t) v))
			printf("*** msb((uintmax_t) %lu) is broken!\n", (unsigned long) v);
		if (msb_twiddle(v) != msb_OlgN_fbr(v))
			printf("*** msb_OlgN_fbr(%lu) is broken!\n", (unsigned long) v);
		if (msb_twiddle(v) != msb_OlgN_sbr(v))
			printf("*** msb_OlgN_sbr(%lu) is broken!\n", (unsigned long) v);
		puts("--------");
	}

	exit(0);
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
