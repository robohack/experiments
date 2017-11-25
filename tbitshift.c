#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

int	main(void);

int
main(void)
{
	volatile unsigned int ui;
	volatile unsigned long ul;
	volatile unsigned long long ull;
	volatile unsigned char uc;
	unsigned int bits;

	bits = 1;
	while ((~0U & (~0U >> (bits - 1))) != 1) {
		bits++;
	}
	printf("The compiler says CHAR_BIT == %d (uint bits = %d)\n", CHAR_BIT, bits);
	if (bits % sizeof(unsigned int) != 0) {
		puts("That's odd -- something is wonky with bit-counting!!!");
	} else if (bits / sizeof(unsigned int) != CHAR_BIT) {
		puts("That's odd -- something is wonky with CHAR_BIT!!!");
	} else if (bits % sizeof(unsigned int) == 0) {
		printf("We calculate CHAR_BIT == %lu\n", bits / sizeof(unsigned int));
	}
	putchar('\n');

	ul = 240;
	printf("starting with %lu\n", ul);
	ul <<= 1;
	printf("<<=1 gives %lu\n", ul);
	putchar('\n');

	/*
	 * NOTE:  A standard C compiler will probably not ever rotate the 1
	 * back into the zero'th bit position -- instead it should just always
	 * shift zeros in and thus always end up with a zero value.
	 *
	 * HOWEVER:  from C99 6.5.7 Bitwise shift operators:
	 *
	 * If the value of the right operand is negative or is greater than or
	 * equal to the width of the promoted left operand, the behavior is
	 * undefined.
	 */

	/*
	 * XXX it seems impossible to get rid of the compiler warning on the
	 * right-hand term of the following expression.
	 *
	 * (clang points at the CHAR_BIT macro as the offending value even
	 * though it expands to "8", which should be a standard integer
	 * constant)
	 */
	uc = ((unsigned char) '\001') << ((unsigned char) sizeof(uc) * (unsigned char) CHAR_BIT);
	printf("the compiler says 1U << %lu == %u\n", (unsigned long) (sizeof(uc) * CHAR_BIT), (unsigned int) uc);
	if (uc != 0) {
		puts("That's odd -- your compiler can't rotate zeros into u_char-sized things!");
	}
	/*
	 * the CPU will probably not rotate this properly since the rotate will
	 * likely be done in an int-sized register, then masked off by the
	 * implicit cast.
	 */
	bits = sizeof(uc) * CHAR_BIT;
	uc = ((unsigned char) 1U) << bits; /* warning: left shift count >= width of type */
	printf("the CPU says 1U << %u == %u%s\n", bits, (unsigned int) uc,
	       uc == 0 ? "" : " That's odd!!! Your CPU can fully rotate a u_char!");
	switch (uc) {
	case 0:
		puts("The CPU cannot fully rotate a u_char, as expected.");
		break;
	case 1:
		puts("That's odd.  The CPU managed to rotate a u_char. Yay!");
		break;
	default:
		puts("Something is drastically wrong?");
	}
	putchar('\n');

	ui = 1U << (sizeof(ui) * CHAR_BIT); /* warning: left shift count >= width of type */	/* UBSAN runtime error:  shift exponent 32 is too large for 32-bit type 'unsigned int' */
	printf("the compiler says 1U << %lu == %u\n", (unsigned long) (sizeof(ui) * CHAR_BIT), ui);
	if (ui != 0) {
		puts("That's odd -- your compiler can't rotate zeros into u_int-sized things!");
	}
	/*
	 * the CPU should always rotate this properly
	 */
	bits = sizeof(ui) * CHAR_BIT;
	ui = 1U << bits;	/* UBSAN runtime error:  shift exponent 32 is too large for 32-bit type 'unsigned int' */
	printf("the CPU says 1U << %u == %u\n", bits, ui);
	switch (ui) {
	case 0:
		puts("That's odd.  The CPU failed to fully rotate a u_int.");
		break;
	case 1:
		puts("The CPU fully rotates a u_int, as expected.");
		break;
	default:
		puts("Something is drastically wrong?");
	}
	putchar('\n');

	ul = 1UL << (sizeof(ul) * CHAR_BIT); /* warning: left shift count >= width of type */	/* UBSAN runtime error:  shift exponent 64 is too large for 64-bit type 'unsigned long' */
	printf("the compiler says 1UL << %lu == %lu\n", (unsigned long) (sizeof(ul) * CHAR_BIT), ul);
	if (ul != 0) {
		puts("That's odd -- your compiler can't rotate zeros into u_long-sized things!");
	}
	/*
	 * the CPU may rotate this properly, depending on the size of long
	 */
	bits = sizeof(ul) * CHAR_BIT;
	ul = 1UL << bits;	/* UBSAN runtime error:  shift exponent 64 is too large for 64-bit type 'unsigned long' */
	printf("the CPU says 1UL << %u == %lu\n", bits, ul);
	switch (ul) {
	case 0:
		if (sizeof(int) == sizeof(ul))
			puts("That's odd.  The CPU failed to fully rotate an int-sized u_long.");
		else
			puts("The CPU cannot fully rotate a u_long.");
		break;
	case 1:
		if (sizeof(int) == sizeof(ul))
			puts("The CPU fully rotates an int-sized u_long.");
		else
			puts("The CPU fully rotates a u_long");
		break;
	default:
		puts("Something is drastically wrong?");
	}
	putchar('\n');

	ull = 1ULL << (sizeof(ull) * CHAR_BIT); /* warning: left shift count >= width of type */	/* UBSAN runtime error:  shift exponent 64 is too large for 64-bit type 'unsigned long long' */
	printf("the compiler says 1ULL << %lu == %llu\n", (unsigned long) (sizeof(ull) * CHAR_BIT), ull);
	if (ull != 0)
		puts("That's odd -- your compiler can't rotate zeros into u_longlong-sized things!");
	/*
	 * the CPU may rotate this properly, depending on the size of long long
	 */
	bits = sizeof(ull) * CHAR_BIT;
	ull = 1ULL << bits;	/* UBSAN runtime error:  shift exponent 64 is too large for 64-bit type 'unsigned long long' */
	printf("the CPU says 1ULL << %u == %llu\n", bits, ull);
	switch (ull) {
	case 0:
		if (sizeof(int) == sizeof(ull))
			puts("That's odd.  The CPU failed to fully rotate an int-sized u_longlong.");
		else
			puts("The CPU cannot fully rotate a u_longlong.");
		break;
	case 1:
		if (sizeof(int) == sizeof(ull))
			puts("The CPU fully rotates an int-sized u_longlong.");
		else
			puts("The CPU fully rotates a u_longlong");
		break;
	default:
		puts("Something is drastically wrong?");
	}
	putchar('\n');

	exit(0);
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm " fn "; " (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
