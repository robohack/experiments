#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


char *binfmt(uintmax_t, unsigned, unsigned, int);
/*
 * binfmt() - Returns a pointer to a string containing the binary formatted
 * representation of an unsigned max-sized int value 'v', zero-padded to 'sz'
 * bits, with 'sc' (as a char) placed every 'sep' digits.
 */
char *
binfmt(uintmax_t x,
       unsigned sz,
       unsigned sep,
       int sc)				/* natural promoted type of char */
{
	static char s[(sizeof(x) * CHAR_BIT * 2) + 1] = {'\0'};
	char *p = s + sizeof(s) - 1;

	if (sz == 0) {
		sz = sizeof(x) * CHAR_BIT;
	}

	while (sz > 0) {
		if (p < s+sizeof(s)-1 && sep > 0 && sz % sep == 0) {
			*--p = (char) (sc & 0xff);
		}
		*--p = (x & 1) ? '1' : '0';
		x >>= 1;
		sz--;
	}

	return p;
}

char *binary_fmt(uintmax_t, int);
/*
 * binary_fmt() - Returns a pointer to a string containing the binary formatted
 * representation of an unsigned max-sized int value 'v', optionally zero-padded
 * to however many bits are in a uintmax_t (no separator character support)
 */
char *
binary_fmt(uintmax_t x,
           int zf)			/* natural promoted type of bool */
{
	static char s[(sizeof(x) * CHAR_BIT) + 1] = {'\0'};
	char *p = s + sizeof(s) - 1;

	*p = '\0';
	do {
#if 0	/* oh wonderful compiler, which is best? */
		*--p = '0' + x % 2;
#else
		*--p = (char) ('0' + (x & 1));
#endif
#if 0	/* oh wonderful compiler, which is best? */
		x /= 2;
#else
		x >>= 1;
#endif
	} while (x);

	if (zf) {
		while (p > s) {
			*--p = '0';
		}
	}

	return p;
}

uintmax_t just_lsb(uintmax_t);
/*
 * Leave just the Least Significant 1 Bit set:
 *
 * This can be useful for extracting the lowest set element of a bit set.
 */
uintmax_t
just_lsb(uintmax_t v)
{
	/*
	 * XXX CPP expressions _should_ be calculated in the target host's
	 * arithmetic system, but will this be?  Is there a cross-compiler
	 * somewere for a 1's complement machine to test this with?
	 *
	 * C99 mandates that all preprocessor arithmetic be performed using the
	 * largest integer type found on the target computer, but as far as I
	 * can tell it does not mandate that the same type of arithmetic be
	 * used.
	 */
#if (INT_MIN != -INT_MAX)
# if 1
	/*
	 * Given a 2's complement binary integer value x, (x&-x) leaves just
	 * the least significant 1 bit set.  (This was pointed-out by Tom May.)
	 *
	 * The reason this works is that it is equivalent to (x & ((~x) + 1));
	 *
	 * any trailing zero bits in x become ones in ~x, adding 1 to that
	 * carries into the following bit, thus flipping it, and so a logical
	 * AND with the original x yields only the flipped bit... the original
	 * position of the least significant 1 bit.
	 */
	return (v & -v);
# else
	/*
	 * Alternatively, but perhaps with one more instruction to execute,
	 * since (x&(x-1)) is actually x stripped of its least significant 1
	 * bit, the least significant 1 bit is also (x^(x&(x-1))).
	 *
	 * This only works on a 2's complement system too
	 */
	return (v ^ (v & (v - 1)));
# endif
#else
# error "just_lsb() not yet implemented for 1's complement machines"
#endif
}

static const char *
ordinal_suff(unsigned int num)
{
	if (num % 100 >= 11 && num % 100 <= 13) {
		return "th";
	}
	switch (num % 10) {
	case 1:
		return "st";
	case 2:
		return "nd";
	case 3:
		return "rd";
	}

	return "th";
}

int	main(void);

/*
 * Most Significant 1 Bit (unrolled?):
 *
 * aka "log base 2"
 *
 * a "binary search" algorithm using shift and compare operations
 *
 * The "log base 2" of an integer is the same as the position of the highest bit
 * (or most significant bit) set, the MSB.
 *
 * Note that this returns –1 if given the invalid input of 0.
 */
u_int ilog2msb(uintmax_t);

u_int
ilog2msb(uintmax_t v)
{
	u_int b = (u_int) -(v == 0);

	/*
	 * N.B.:  this gets "warning: left shift count >= width of type"; and
	 * "warning: comparison of unsigned expression >= 0 is always true"; and
	 * "warning: right shift count >= width of type"
	 *
	 * ... all for the highest step (normally step(64))
	 *
	 * XXX It's funny a smart enough compiler can't admit that it can reduce
	 * (sizeof(blah) > 8) to a constant at compile time.  The warning should
	 * be "constant expression is always false"!
	 */
#define step(x)	if (v >= ((uintmax_t) 1) << (x))			\
			b += (x), v >>= (x)

#ifdef __SIZEOF_LONG_LONG__
/*
 * xxx assume intmax_t is long long, _and_ 128 bits!
 */
# if __SIZEOF_LONG_LONG__ > __SIZEOF_LONG__
	if (sizeof(uintmax_t) > 8)
		step(64);
# endif
#endif
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

unsigned int ilog2(uintmax_t);

unsigned int
ilog2(uintmax_t v)
{
#ifndef __has_builtin
# define __has_builtin(x) 0  /* for compatibility */
#endif

#if __has_builtin(__builtin_clz)
	return ((sizeof(uintmax_t) * CHAR_BIT) - 1) ^ __builtin_clzll(v);
#else
	return ilog2msb(v);
#endif
}

static unsigned int
ilog10(uintmax_t v)
{
	unsigned int r;
	static unsigned long long int const PowersOf10[] =
		{ 1LLU, 10LLU, 100LLU, 1000LLU, 10000LLU, 100000LLU, 1000000LLU,
		  10000000LLU, 100000000LLU, 1000000000LLU, 10000000000LLU,
		  100000000000LLU, 1000000000000LLU, 10000000000000LLU,
		  100000000000000LLU, 1000000000000000LLU, 10000000000000000LLU,
		  100000000000000000LLU, 1000000000000000000LLU,
		  10000000000000000000LLU };

	if (!v)
		return ~0U;

	/*
	 * By the relationship "log10(v) = log2(v) / log2(10)", we need to
	 * multiply "log2(v)" by "1 / log2(10)", which is approximately
	 * 1233/4096, or (1233, followed by a right shift of 12).
	 *
	 * Finally, since the result is only an approximation that may be off
	 * by one, the exact value is found by subtracting "v < PowersOf10[r]"
	 * from the result.
	 */
	r = ((ilog2(v) * 1233) >> 12) + 1;

	return r - (v < PowersOf10[r]);
}

int
main(void)
{
	volatile uintmax_t ui;
	volatile unsigned int bits;

	printf("(1<<10)-1) = %s\n", binary_fmt((1ULL<<10)-1, false));

	for (bits = 0; bits < sizeof(ui) * CHAR_BIT; bits++) {
		uintmax_t result;

		/*
		 * two ways to create a mask containing only the N bit
		 */
		if (INT_MIN != -INT_MAX) {    /* It's a two's complement */
			/*
			 * Compute 2^bits with bit-wise left-shift, complement, and
			 * minus and unary minus
			 *
			 * note "result" can be either signed or unsigned, but this
			 * must be done on a two's-complement machine
			 */
			result = ~((1ULL << bits) - 1);
			result = (result & -result);
			/*
			 * Is it really a Power of 2?
			 *
			 * A non-negative binary integer value is a power of 2 if the
			 * following is true, using 2's complement arithmetic.
			 */
			if (result & (result - 1))
				printf("the 2's complement way of computing 2^bits failed!  (0x%016jx, %s)\n",
				       result, binary_fmt(result, true));
		} else {
			unsigned int exp = bits - 1;

			result = 1;
			while (exp--) {
				result *= 2;
			}
		}
		printf("only %02u%s bit off: 0x%0*jx 0%0*jo %s\n",
		       bits,
		       ordinal_suff(bits),
		       (int) (sizeof(result) * CHAR_BIT + 3) / 4, ~result, /* 4 = bits per hexidecimal digit */
		       (int) (sizeof(result) * CHAR_BIT + 3) / 3, ~result, /* 3 = bits per octal digit */
		       binfmt(~result, (unsigned) sizeof(result) * CHAR_BIT, 4, ' '));
		printf("only %02u%s bit on : 0x%0*jx 0%0*jo %s\n",
		       bits,
		       ordinal_suff(bits),
		       (int) (sizeof(result) * CHAR_BIT + 3) / 4, result, /* 4 = bits per hexidecimal digit */
		       (int) (sizeof(result) * CHAR_BIT + 3) / 3, result, /* 3 = bits per octal digit */
		       binfmt(result, (unsigned) sizeof(result) * CHAR_BIT, 4, ' '));


		/*
		 * 2 ways to create a mask to turn off the N high-order bits
		 */
		/* IMPORTANT:  0ULL must be an unsigned int constant */
		ui = ~0ULL >> bits;
		printf("1st %02u bit%s  : 0x%0*jx 0%0*jo %s\n",
		       bits,
		       bits > 1 ? "s off" : " off ",
		       (int) (sizeof(ui) * CHAR_BIT + 3) / 4, ui, /* 4 = bits per hexidecimal digit */
		       (int) (sizeof(ui) * CHAR_BIT + 3) / 3, ui, /* 3 = bits per octal digit */
		       binfmt(ui, (unsigned) sizeof(ui) * CHAR_BIT, 4, ' '));

		if (bits > 0 && INT_MIN != -INT_MAX){
			/* must be on a two's-complement machine */
			/* fails if bits == 0 */
			ui = (1ULL << (((sizeof(ui) * CHAR_BIT) - bits))) - 1;
			printf("alt 1st %02u off   : 0x%0*jx 0%0*jo %s\n",
			       bits,
			       (int) (sizeof(ui) * CHAR_BIT + 3) / 4, ui, /* 4 = bits per hexidecimal digit */
			       (int) (sizeof(ui) * CHAR_BIT + 3) / 3, ui, /* 3 = bits per octal digit */
			       binfmt(ui, (unsigned) sizeof(ui) * CHAR_BIT, 4, ' '));
		}

		/*
		 * 2 ways to create a mask to turn off the N low-order bits
		 */
		if (bits > 0) {
			/* IMPORTANT:  0ULL must be an unsigned int constant */
			/* fails if bits == 0 */
			ui = ~(~0ULL >> ((sizeof(ui) * CHAR_BIT) - bits));
			printf("low %02u bit%s  : 0x%0*jx 0%0*jo %s\n",
			       bits,
			       bits > 1 ? "s off": " off ",
			       (int) (sizeof(ui) * CHAR_BIT + 3) / 4, ui, /* 4 = bits per hexidecimal digit */
			       (int) (sizeof(ui) * CHAR_BIT + 3) / 3, ui, /* 3 = bits per octal digit */
			       binfmt(ui, (unsigned) sizeof(ui) * CHAR_BIT, 4, ' '));
		}

		if (INT_MIN != -INT_MAX) {
			/* must be on a two's-complement machine */
			ui = ~((1ULL << bits) - 1);
			printf("alt low %02u off   : 0x%0*jx 0%0*jo %s\n",
			       bits,
			       (int) (sizeof(ui) * CHAR_BIT + 3) / 4, ui, /* 4 = bits per hexidecimal digit */
			       (int) (sizeof(ui) * CHAR_BIT + 3) / 3, ui, /* 3 = bits per octal digit */
			       binfmt(ui, (unsigned) sizeof(ui) * CHAR_BIT, 4, ' '));
		}
		putchar('\n');
	}

	printf("%*s  %*s  %-*s\n",
	       (int) (sizeof(ui) * CHAR_BIT), "high-N-bits-mask",
	       ilog10(UINTMAX_MAX), "LSB-decimal",
	       (int) (sizeof(ui) * CHAR_BIT), "Just the LSB");
	for (bits = 0; bits < sizeof(ui) * CHAR_BIT; bits++) {
		char *uib;

		ui = ~((1ULL << bits) - 1);
		uib = strdup(binary_fmt(ui, true));
		printf("%s  %*ju  %s\n",
		       uib,
		       ilog10(UINTMAX_MAX), just_lsb(ui),
		       binary_fmt(just_lsb(ui), true));
		free(uib);
	}

	exit(0);
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm -f " fn "; " (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
