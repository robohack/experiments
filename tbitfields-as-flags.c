#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * Bitfields are essentially self-documenting and make it very clear what
 * values are being set and how they are being set.  There's no need to parse
 * through both the binary logic operators used in traditional masking
 * instructions, and also perhaps the definitions of the macros used.
 *
 * Indeed straight-forward code using bit-fields is often much clearer than the
 * equivalent explicit binary operations!  (see any termios(4) manipulating
 * code for an excellent example of obtuse logic)
 *
 * Debuggers will, or should, show bitfields symbolically.
 *
 * Bitfields cannot be used with binary (bitwise) operators, but in theory it
 * would be portable to do such operations on an unsigned int in a union with
 * an exactly int-sized bitfield group.  Wanting to do so though suggests
 * serious design flaws in your data structures.
 *
 * You can't have arrays of bit-fields, but you can have arrays of structs
 * containing bitfields.
 *
 * Non-int-sized bit fields are not portable, but GCC and some other compilers
 * allow any integer type to be used.
 *
 * It has been claimed by some that compilers are often generate extremely poor
 * code to handle bitfields, particularly for this kind of purpose.  This is of
 * course just anecdotal.
 *
 * Bit-fields are inconvenient when you also want to be able to manipulate some
 * collection of bits as a whole (perhaps to copy a set of flags, though of
 * course you can use struct assignment to do much the same when the types are
 * equivalent).  Again you can also use a same-sized int in a union with the
 * bitfield to access the same bits as one group.
 *
 * For a way of using initialized enum values to specifiy bit flags for
 * anachronistic APIs, see tenum.c (enums are always better than CPP macros,
 * but bitfields are always better than manual bit flags in integers of any
 * kind!!!)
 */

/*
 * All three of these bits belong to the same 'unsigned int' variable.  They
 * are convenient to flip on and off and can be changed individually without
 * any chance of interfering with any other flags in the same "object".
 */
struct fruit_to_buy {
	unsigned int banana:1;
	unsigned int apple:1;
	unsigned int pear:1;
};

/*
 * using 'bool' bitfields works with gcc and clang -- it may save space, but it
 * doesn't really add anything, and it may confuse some folks
 *
 * (it may really confuse anyone with a C90 or earlier compiler and has defined
 * 'bool' as 'int' (instead of 'unsigned int'))
 */
struct main_opts {
	bool opt_one:1;
	bool opt_two:1;
};

struct bool_opts {
	bool opt_one;
	bool opt_two;
};
struct bool_opts1 {
	bool opt_one;
};
struct bool_opts3 {
	bool opt_one;
	bool opt_two;
	bool opt_three;
};
struct bool_opts4 {
	bool opt_one;
	bool opt_two;
	bool opt_three;
	bool opt_four;
};
struct bool_opts5 {
	bool opt_one;
	bool opt_two;
	bool opt_three;
	bool opt_four;
	bool opt_five;
};
struct bool_opts6 {
	bool opt_one;
	bool opt_two;
	bool opt_three;
	bool opt_four;
	bool opt_five;
	bool opt_six;
};
struct bool_opts7 {
	bool opt_one;
	bool opt_two;
	bool opt_three;
	bool opt_four;
	bool opt_five;
	bool opt_six;
	bool opt_seven;
};
struct bool_opts8 {
	bool opt_one;
	bool opt_two;
	bool opt_three;
	bool opt_four;
	bool opt_five;
	bool opt_six;
	bool opt_seven;
	bool opt_eight;
};

struct int_opts {
	int opt_one:1;	/* xxx using 'int' here shows why unsigned is needed */
	int opt_two:1;
};

struct uint_opts {
	unsigned int opt_one:1;
	unsigned int opt_two:1;
};

int main(int, char *[]);
struct fruit_to_buy setcart(struct fruit_to_buy);

int
main(int argc,
     char *argv[])
{
	struct main_opts opts = {true,
	                         false};
	struct bool_opts bopts = {false,
	                          false};
	struct int_opts opts2 = {false,
	                         true};	/* xxx warning: conversion to ‘signed char:1’ alters ‘int’ constant value [-Wconversion] */
	struct uint_opts opts3 = {true,
	                          true};
	/*
	 * note how simple it should be to initialise defaults
	 *
	 * XXX however clang doesn't like this and warns "missing field 'apple'
	 * initializer" (ignoring 'pear' entirely it seems!)
	 */
	struct fruit_to_buy cart = {false};
	struct fruit_to_buy everyone[10] = {{false}}; /* XXX same clang apple but no pear warning here */
	struct fruit_to_buy cart2 = {
		/* no warning from clang here about missing bananas! */
		.apple = false,
		.pear = true};

	if (argc > 1) {
		opts.opt_one = false;
		opts.opt_two = true;
		bopts.opt_one = true;
		bopts.opt_two = true;
		opts2.opt_one = true;	/* xxx warning: conversion to ‘signed char:1’ alters ‘int’ constant value [-Wconversion] */
		opts2.opt_two = false;
		opts3.opt_one = false;
		opts3.opt_two = false;
	}
	(void) argv;

	cart = setcart(cart); /* NOTE: GCC's -Waggregate-return will warn "function call has aggregate value" */

	printf("sizeof(bool:1) = %lu\n", sizeof(struct main_opts));
	printf("sizeof(bool) = %lu\n", sizeof(struct bool_opts));
	printf("sizeof(int:1) = %lu\n", sizeof(struct int_opts));
	printf("sizeof(uint:1) = %lu\n", sizeof(struct uint_opts));

	printf("sizeof(bool1-8) = %lu, %lu, %lu, %lu, %lu, %lu, %lu, %lu\n",
	       sizeof(struct bool_opts1),
	       sizeof(struct bool_opts),
	       sizeof(struct bool_opts3),
	       sizeof(struct bool_opts4),
	       sizeof(struct bool_opts5),
	       sizeof(struct bool_opts6),
	       sizeof(struct bool_opts7),
	       sizeof(struct bool_opts8));

	printf("do you have bananas?  %s\n", everyone[0].banana ? "yes" : "no");
	printf("do you have apples?  %s\n", everyone[0].apple ? "yes" : "no");
	printf("do you have pears?  %s\n", everyone[0].pear ? "yes" : "no");

	everyone[2] = cart2;

	printf("do you like bananas?  %s\n", everyone[2].banana ? "yes" : "no");
	printf("do you like apples?  %s\n", everyone[2].apple ? "yes" : "no");
	printf("do you like pears?  %s\n", everyone[2].pear ? "yes" : "no");

	/* one can also pass a literal with initializer... */

	cart = setcart((struct fruit_to_buy) { /* NOTE: GCC's -Waggregate-return will warn "function call has aggregate value" */
			.banana = true,
			.apple = true,
			.pear = false
		       });

	if (opts.opt_one) {
		everyone[3] = setcart(cart);
	} else {
		everyone[3] = setcart((struct fruit_to_buy) {
				.banana = bopts.opt_two,
				.apple = opts2.opt_two, /* XXX CLANG: implicit conversion changes signedness: 'int' to 'unsigned int' [-Wsign-conversion]; GCC: conversion to ‘unsigned char:1’ from ‘signed char:1’ may change the sign of the result [-Wsign-conversion] */
				.pear = opts3.opt_two}
			);
	}

	exit(0);
}

struct fruit_to_buy
setcart(struct fruit_to_buy cart)
{ /* NOTE: GCC's -Waggregate-return will warn "returns an aggregate", duh! */
	cart.banana = true;
	cart.apple = false;
	cart.pear = true;
/*
 * As opposed to say:
 */
#if 0
	cart.buy |= FLAG_BANANA | FLAG_PEAR;
	cart.buy &= ~FLAG_APPLE;
#endif

	return cart;
}

/*
 * XXX Good compilers will in-line setcart() with any optimization enabled.
 *
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm -f " fn "; " (default-value 'compile-command) " OPTIM=-O0 " fn " && ./" fn))
 * End:
 */
