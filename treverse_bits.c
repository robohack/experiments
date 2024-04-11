#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

unsigned int
rev_bits_swar_w4c(unsigned int x) /* "SIMD Within A Register */
{
#if 0
	assert(sizeof(unsigned int) == 4);
#endif
	x = ((x >> 1) & 0x55555555u) | ((x & 0x55555555u) << 1);
	x = ((x >> 2) & 0x33333333u) | ((x & 0x33333333u) << 2);
	x = ((x >> 4) & 0x0f0f0f0fu) | ((x & 0x0f0f0f0fu) << 4);
	x = ((x >> 8) & 0x00ff00ffu) | ((x & 0x00ff00ffu) << 8);
	x = (x >> 16) | (x << 16);

	return x;
}


unsigned int
rev_bits_swar_w8c(unsigned int x)
{
#if 0
	assert(sizeof(unsigned int) == 4);
#endif
	x = (((x & 0xaaaaaaaa) >> 1) | ((x & 0x55555555) << 1));
	x = (((x & 0xcccccccc) >> 2) | ((x & 0x33333333) << 2));
	x = (((x & 0xf0f0f0f0) >> 4) | ((x & 0x0f0f0f0f) << 4));
	x = (((x & 0xff00ff00) >> 8) | ((x & 0x00ff00ff) << 8));
	x = (x >> 16) | (x << 16);

	return x;
}


unsigned int
rev_bits_one_by_one(unsigned int input)
{
	unsigned int output;
	unsigned int i;			/* uint_fast8_t */

	for (output = 0, i = 0; i < (sizeof(input) << 3); i++) {
		if ((input >> i) & 1) {
			output |= (1 << ((sizeof(input) << 3) - 1 - i));
		}
	}
	return output;
}

unsigned int
rev_bits_one_by_one_n(unsigned int input,
		      unsigned int bits)
{
	unsigned int output;
	unsigned int i;			/* uint_fast8_t */

	for (output = 0, i = 0; i < bits; i++) {
		if ((input >> i) & 1) {
			output |= (1 << (bits - 1 - i));
		}
	}
	return output;
}


unsigned int
rev_bits_shift_both(unsigned int input)
{
	unsigned int output;
	unsigned int i;			/* uint_fast8_t */

	for (output = 0, i = 0;
	     i < (sizeof(unsigned int) << 3) - 1;
	     i++) {
		output |= (input & 1);
		input >>= 1;
		output <<= 1;
	}
	output |= (input & 1);

	return output;
}

unsigned int
rev_bits_shift_both_n(unsigned int input,
		      unsigned int bits)
{
	unsigned int output;
	unsigned int i;			/* uint_fast8_t */

	for (output = 0, i = 0;
	     i < (bits - 1);
	     i++) {
		output |= (input & 1);
		input >>= 1;
		output <<= 1;
	}
	output |= (input & 1);

	return output;
}

unsigned int
rev_bits_shift_both_opt(unsigned int input)
{
	unsigned int output = input;		    /* xxx (input & 1) */
	unsigned int s = (sizeof(output) << 3) - 1;	/* uint_fast8_t */

	input >>= 1;			/* we already have the LSB */
	while (input > 0) {
		output <<= 1;
		output |= input & 1;
		s--;
		input >>= 1;
	}
	output <<= s;	      /* insert highest zero bits from input */

	return output;
}

unsigned int
rev_bits_shift_both_opt_n(unsigned int input,
			  unsigned int bits)
{
	unsigned int output = (input & 1);
	unsigned int s = bits - 1;	/* uint_fast8_t */

	input >>= 1;			/* we already have the LSB */
	while (input > 0 && s > 0) {
		output <<= 1;		/* make room for LSB */
		output |= input & 1;	/* copy LSB */
		s--;
		input >>= 1;		/* drop LSB */
	}
	output <<= s;	      /* insert highest zero bits from input */

	return output;
}


unsigned int
rev_bits_shift_mask(unsigned int input)
{
	unsigned int output = 0;
	unsigned int mask = 1U << ((sizeof(input) << 3) - 1); /* set the high bit */
	unsigned int setter = 1;

	while (mask > 0) {
		if (input & mask ) {
			output |= setter;
		}
		mask >>= 1;
		setter <<= 1;
	}

	return output;
}

unsigned int
rev_bits_shift_mask_n(unsigned int input,
		      unsigned int bits)
{
	unsigned int output = 0;
	unsigned int mask = 1U << (bits - 1); /* set the high bit */
	unsigned int setter = 1;

	while (mask > 0) {
		if (input & mask ) {
			output |= setter;
		}
		mask >>= 1;
		setter <<= 1;
	}

	return output;
}


char *binary_fmt(unsigned int, int);

char *
binary_fmt(unsigned int x,
	   int zf)			/* natural promoted type of bool  */
{
	static char str[(sizeof(x) << 3) + 1];
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

int
main(void);

int
main()
{
	unsigned int uis[] = { 0x12345678,
			       0x9ABCDEF0,
			       0x0FEDCBA9,
			       0x87654321,
			       0x0f0f0f0f,
			       0xf0f0f0f0,
			       0x33333333,
			       0x77777777,
			       0x55555555,
			       0xAAAAAAAA};
	unsigned int i;

	for (i = 0; i < sizeof(uis) / sizeof(uis[0]); i++) {
		unsigned int ui = uis[i];
		unsigned int rui;

		printf("ui = 0x%08x = %32s\n", ui, binary_fmt(ui, true));

		rui = rev_bits_swar_w4c(ui);
		printf("     0x%08x = %32s\n", rui, binary_fmt(rui, true));

		rui = rev_bits_swar_w8c(ui);
		printf("     0x%08x = %32s\n", rui, binary_fmt(rui, true));

		rui = rev_bits_one_by_one(ui);
		printf("     0x%08x = %32s\n", rui, binary_fmt(rui, true));

		rui = rev_bits_shift_both(ui);
		printf("     0x%08x = %32s\n", rui, binary_fmt(rui, true));

		rui = rev_bits_shift_both_opt(ui);
		printf("     0x%08x = %32s\n", rui, binary_fmt(rui, true));

		rui = rev_bits_shift_mask(ui);
		printf("     0x%08x = %32s\n", rui, binary_fmt(rui, true));

		printf("\n");
	}
	for (i = 0; i < sizeof(uis) / sizeof(uis[0]); i++) {
		unsigned int ui = uis[i];
		unsigned int rui;

		printf("ui = 0x%08x = %32s\n", ui, binary_fmt(ui, true));

		rui = rev_bits_one_by_one_n(ui, 13);
		printf("     0x%08x = %32s\n", rui, binary_fmt(rui, true));

		rui = rev_bits_shift_both_n(ui, 13);
		printf("     0x%08x = %32s\n", rui, binary_fmt(rui, true));

		rui = rev_bits_shift_both_opt_n(ui, 13);
		printf("     0x%08x = %32s\n", rui, binary_fmt(rui, true));

		rui = rev_bits_shift_mask_n(ui, 13);
		printf("     0x%08x = %32s\n", rui, binary_fmt(rui, true));

		printf("\n");
	}

	exit(0);
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm -f " fn "; " (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
