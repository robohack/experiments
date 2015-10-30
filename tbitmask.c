#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *binary_fmt(unsigned int, int);

char *
binary_fmt(unsigned int x,
           int zf)			/* natural promoted type of bool  */
{
	static char str[(sizeof(x) * CHAR_BIT) + 1];
	char *s = str + sizeof(str) - 1;

	*str = *s = '\0';
	do {
#if 0	/* oh wonderful compiler, which is best? */
		*--s = '0' + x % 2;
#else
		*--s = '0' + (x & 1);
#endif
#if 0	/* oh wonderful compiler, which is best? */
		x /= 2;
#else
		x >>= 1;
#endif
	} while (x);

	if (zf) {
		while (s > str)
			*--s = '0';
	}

	return s;
}

int just_lsb(uintmax_t);
/*
 * Leave just the Least Significant 1 Bit set:
 *
 * This can be useful for extracting the lowest set element of a bit set.
 */
int
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
	assert(INT_MIN != -INT_MAX);
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
	return (int ) (v & -v);
# else
	/*
	 * Alternatively, but perhaps with one more instruction to execute,
	 * since (x&(x-1)) is actually x stripped of its least significant 1
	 * bit, the least significant 1 bit is also (x^(x&(x-1))).
	 *
	 * This only works on a 2's complement system too
	 */
	return (int) (v ^ (v & (v - 1)));
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

int
main(void)
{
	volatile unsigned int ui;
	volatile unsigned int bits;

	printf("(1<<10)-1) = %s\n", binary_fmt((1<<10)-1, false));

	for (bits = 0; bits <= sizeof(int) * CHAR_BIT; bits++) {
		unsigned int result;

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
			result = ~((1U << bits) - 1);
			result = (result & -result);
			/*
			 * Is it really a Power of 2?
			 *
			 * A non-negative binary integer value is a power of 2 if the
			 * following is true, using 2's complement arithmetic.
			 */
			if (result & (result - 1))
				printf("the 2's complement way of computing 2^bits failed!  (0x%08x, %s)\n",
				       result, binary_fmt(result, false));
		} else {
			unsigned int exp = bits - 1;

			result = 1;
			while (exp--) {
				result *= 2;
			}
		}
		printf("only %02u%s bit off: 0x%08x 0%011o %s\n",
		       bits, ordinal_suff(bits), ~result, ~result, binary_fmt(~result, true));
		printf("only %02u%s bit on : 0x%08x 0%011o %s\n",
		       bits, ordinal_suff(bits), result, result, binary_fmt(result, true));


		/*
		 * 2 ways to create a mask to turn off the N high-order bits
		 */
		/* IMPORTANT:  0U must be an unsigned int constant */
		ui = ~0U >> bits;
		printf("1st %02u bit%s  : 0x%08x 0%011o %s\n",
		       bits, bits > 1 ? "s off" : " off ", ui, ui, binary_fmt(ui, true));

		if (bits > 0 && INT_MIN != -INT_MAX){
			/* must be on a two's-complement machine */
			/* fails if bits == 0 */
			ui = (1U << (((sizeof(ui) * CHAR_BIT) - bits))) - 1;
			printf("alt 1st %02u off   : 0x%08x 0%011o %s\n",
			       bits, ui, ui, binary_fmt(ui, true));
		}

		/*
		 * 2 ways to create a mask to turn off the N low-order bits
		 */
		if (bits > 0) {
			/* IMPORTANT:  0U must be an unsigned int constant */
			/* fails if bits == 0 */
			ui = ~(~0U >> ((sizeof(ui) * CHAR_BIT) - bits));
			printf("low %02u bit%s  : 0x%08x 0%011o %s\n",
			       bits, bits > 1 ? "s off": " off ", ui, ui, binary_fmt(ui, true));
		}

		if (INT_MIN != -INT_MAX) {
			/* must be on a two's-complement machine */
			ui = ~((1U << bits) - 1);
			printf("alt low %02u off   : 0x%08x 0%011o %s\n",
			       bits, ui, ui, binary_fmt(ui, true));
		}
		putchar('\n');
	}

	for (bits = 1; bits < sizeof(int) * CHAR_BIT; bits++) {
		char *uib;

		ui = ~((1U << bits) - 1);
		uib = strdup(binary_fmt(ui, true));
		printf("%s %011u %s\n",
		       uib,
		       just_lsb((uintmax_t) ui),
		       binary_fmt((unsigned) just_lsb((uintmax_t) ui), true));
		free(uib);
	}

	exit(0);
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
