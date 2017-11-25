#include <sys/cdefs.h>

#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*
 * NOTICE:  compile and run this test at least once with -O0.
 *
 * NOTES about C implementations vs. integer overflow detection:
 *
 * GCC has '-ftrapv' which (in some versions) replaces integer arithmetic
 * operators with calls to libgcc wrapper functions which (assuming
 * two's-complement signed integer arithmetic implementstions) will do overflow
 * checks (usually post-condition wrap-around checks) and call abort() if
 * there's an overflow detected in the operation.
 *
 * Unfortunately using GCC-4.1.2's '-ftrapv' in combination with any
 * optimization (i.e. '-O1' or greater) then the compiler will apparently
 * generate code which will purposefuly optimize away some of the libgcc
 * internal calls for addition operations.  Only underflow detection seems to
 * work with this option and with optimization.
 *
 * The tests below will still detect the overflows at runtime, and we know
 * these tests are never optimized away because we check that they work
 * correctly for known non-overflowing values.  This is a GCC bug.
 *
 * Even more recent versions of GCC suffer worse:
 *
 * '-ftrapv' has no affect w/ gcc 4.2.1 (on Mac OS X 10.6.8, not in manpage)
 *
 * '-ftrapv' has no affect w/ gcc 4.6.1 (on Mac OS X 10.6.8, from fink, not in manpage)
 *
 * '-ftrapv' has no affect w/ gcc 4.2.1 (on FreeBSD 8.x, is documented)
 *
 * And as of last check, it's still a noted, open, bug:
 *
 *	http://gcc.gnu.org/bugzilla/show_bug.cgi?id=35412
 *
 *	http://gcc.gnu.org/bugzilla/show_bug.cgi?id=52478
 *
 * However some discussions on some GCC mailing lists suggest this is too
 * broken to fix as-is.  Back in 2008 it was suggested it be deprecated for 4.4
 * and re-implemented properly using GIMPLE, whatever that is.
 *
 *
 * Some other work done on this issue:
 *
 *	http://repository.cmu.edu/cgi/viewcontent.cgi?article=1017&context=ece
 *
 *
 * Clang does support '-ftrapv', and it works, even with -O3, though
 * optimization may cause reordering that may confuse what you see.
 *
 *
 * Someday C may have "as-if infinitely ranging integers".....
 */

/*
 * WARNING:  when testing this kind of stuff one must be careful not to give
 * away the secret to the optimizer, else it's certain to just elide your code.
 * Here we either use a system call to make the compiler uncertain about what
 * value will be used, or (without the following #define) we enable manual
 * input options requiring the user to supply the values, giving our suggestion
 * as a simplistic automated Turing test to out-smart the compiler.
 */
#if 1
#define USE_TIME_TO_INIT	/* defined.... */
#endif

#ifdef USE_TIME_TO_INIT

# define UINT_INIT(UFOO, UBAR)					\
	ufoo = (time((time_t *) NULL) < 0) ? 42U : UFOO;	\
	ubar = (time((time_t *) NULL) < 0) ? 42U : UBAR;

# define SINT_INIT(FOO, BAR)					\
	foo = (time((time_t *) NULL) < 0) ? 42 : FOO;		\
	bar = (time((time_t *) NULL) < 0) ? 42 : BAR;

#else /* !USE_TIME_TO_INIT */

# define UINT_INIT(UFOO, UBAR)			\
	if (ufoo != UFOO) {			\
		printf("Enter %u: ", UFOO);	\
		fscanf(stdin, "%u", &ufoo);	\
		printf("got %u\n\n", ufoo);	\
	}					\
	if (ubar != UBAR) {			\
		printf("Enter %u: ", UBAR);	\
		fscanf(stdin, "%u", &ubar);	\
		printf("got %u\n\n", ubar);	\
	}

# define SINT_INIT(FOO, BAR)			\
	if (foo != FOO) {			\
		printf("Enter %d: ", FOO);	\
		fscanf(stdin, "%d", &foo);	\
		printf("got %d\n\n", foo);	\
	}					\
	if (bar != BAR) {			\
		printf("Enter %d: ", BAR);	\
		fscanf(stdin, "%d", &bar);	\
		printf("got %d\n\n", bar);	\
	}

#endif /* USE_TIME_TO_INIT */


void	unsigned_int_add_ovf(unsigned int, unsigned int);
void	unsigned_int_sub_undf(unsigned int, unsigned int);
void	signed_int_add_ovf_1(signed int, signed int);
void	signed_int_add_ovf_2(signed int, signed int);
void	signed_int_add_ovf_3(signed int, signed int);
void	signed_int_add_ovf_mot(signed int, signed int);
void	signed_int_add_ovf_cert_2s(signed int, signed int);
void	signed_int_add_ovf_cert_any(signed int, signed int);
void	signed_int_sub_undf_3(signed int, signed int);
void	signed_int_sub_undf_mot(signed int, signed int);


int mult(int, int);

#ifndef __noinline
# if defined(__GNUC__)
#  define __noinline	__attribute__((__noinline__))
# else
#  define __noinline	# empty
# endif
#endif

__noinline int
mult(int a,
     int b)
{
	volatile int r;

	r = a * b;

	return r;
}

static void AbortSignalHandler(int);

static void
AbortSignalHandler(int sig __unused)
{
	fputs("Overflow detected at runtime!!!\n", stderr);
}

int main(void);

int
main()
{
	volatile unsigned int ufoo = 0;
	volatile unsigned int ubar = 0;

	volatile unsigned int ures;

	volatile signed int foo = 0;
	volatile signed int bar = 0;

	volatile signed int res;

	int high_int_bit = (1 << ((int) (sizeof(int) * CHAR_BIT) - 1));

	setbuf(stdout, NULL);
	setbuf(stderr, NULL);

	signal(SIGABRT, AbortSignalHandler);

	/*
	 * note: this entire "if" trivially gets optimized away with -O1
	 */
	if (high_int_bit != INT_MIN) {
		printf("*** high bit expression says INT_MIN is busted! (INT_MIN=0x%x)\n", INT_MIN);
	}


	printf("\nTesting native unsigned integer overflow/underflow wrapping....\n\n");

	/*
	 * XXX these should _not_ ever fail even with -ftrapv
	 */

	UINT_INIT(UINT_MAX, 1U);
	ures = ufoo +  ubar;			/* UINT_MAX + 1 */
	printf("unsigned integer subtraction underflow: %u - %u = %u!\n", ufoo, ubar, ures);

	UINT_INIT(UINT_MAX, UINT_MAX);
	ures = ufoo + ubar;			/* UINT_MAX + UINT_MAX */
	printf("unsigned integer subtraction underflow: %u - %u = %u!\n", ufoo, ubar, ures);

	UINT_INIT(0U, 1U);
	ures = ufoo - ubar;			/* 0 - 1 */
	printf("unsigned integer subtraction underflow: %u - %u = %u!\n", ufoo, ubar, ures);

	UINT_INIT(0U, UINT_MAX);
	ures = ufoo - ubar;			/* 0 - UINT_MAX */
	printf("unsigned integer subtraction underflow: %u - %u = %u!\n", ufoo, ubar, ures);

	printf("\nTesting native signed integer addition overflow detection....\n\n");


	/*
	 * these _should_ fail if compiled with -ftrapv
	 */

	SINT_INIT(INT_MAX, 1);
	res = foo + bar;			/* INT_MAX + 1 */
	printf("*** signed integer addition overflow: %d + %d = %d!\n", foo, bar, res);

	SINT_INIT(INT_MAX, INT_MAX);
	res = foo + bar;			/* INT_MAX + INT_MAX */
	printf("*** signed integer addition overflow: %d + %d = %d!\n", foo, bar, res);

	printf("\nTesting native signed integer subtraction underflow detection....\n\n");

	SINT_INIT(INT_MIN, 1);
	res = foo - bar;			/* INT_MIN - 1 */
	printf("*** signed integer subtraction underflow: %d + %d = %d!\n", foo, bar, res);

	SINT_INIT(INT_MIN, INT_MAX);
	res = foo - bar;			/* INT_MIN - INT_MAX */
	printf("*** signed integer subtraction underflow: %d + %d = %d!\n", foo, bar, res);


	printf("\nTesting native signed integer multiplication overflow detection....\n\n");

	SINT_INIT(0, 0);
	res = mult(foo, bar);
	SINT_INIT(0, -1);
	res = mult(foo, bar);
	SINT_INIT(-1, 0);
	res = mult(foo, bar);
	SINT_INIT(-1, -1);
	res = mult(foo, bar);

	SINT_INIT(INT_MAX, 2);
	res = mult(foo, bar);
	printf("*** signed integer multiplication overflow: %d * %d = %d!\n", foo, bar, res);

	SINT_INIT(INT_MAX, -2);
	res = mult(foo, bar);
	printf("*** signed integer multiplication overflow: %d * %d = %d!\n", foo, bar, res);


	/*
	 * Now test various methods of doing manual overflow/underflow detection
	 *
	 * NOTE: some/most/all(?) of the signed checks will fail with a working
	 * runtime -ftrapv enabled as they do the calculation before checking
	 * for overflow....
	 */

	printf("\nTesting unsigned integer OK....\n\n");

	UINT_INIT(1U, 1U);
	unsigned_int_add_ovf(ufoo, ubar);	/* 1 + 1 */

	UINT_INIT((UINT_MAX - 1U), 1U);
	unsigned_int_add_ovf(ufoo, ubar);	/* (UINT_MAX-1) + 1 */


	printf("\nTesting unsigned integer overflow/underflow detection....\n\n");

	UINT_INIT(UINT_MAX, 1U);
	unsigned_int_add_ovf(ufoo, ubar);	/* UINT_MAX + 1 */

	UINT_INIT(UINT_MAX, UINT_MAX);
	unsigned_int_add_ovf(ufoo, ubar);	/* UINT_MAX + UINT_MAX */

	UINT_INIT(0U, 1U);
	unsigned_int_sub_undf(ufoo, ubar);	/* 0 - 1 */

	UINT_INIT(0U, UINT_MAX);
	unsigned_int_sub_undf(ufoo, ubar);	/* 0 - UINT_MAX */


	printf("\nTesting signed integer OK....\n\n");

	SINT_INIT(1, 1);

	signed_int_add_ovf_1(foo, bar);		/* 1 + 1 */
	signed_int_add_ovf_2(foo, bar);
	signed_int_add_ovf_3(foo, bar);
	signed_int_add_ovf_mot(foo, bar);
	signed_int_add_ovf_cert_2s(foo, bar);
	signed_int_add_ovf_cert_any(foo, bar);

	SINT_INIT((INT_MAX - 1), 1);

	signed_int_add_ovf_1(foo, bar);		/* (INT_MAX-1) + 1 */
	signed_int_add_ovf_2(foo, bar);
	signed_int_add_ovf_3(foo, bar);
	signed_int_add_ovf_mot(foo, bar);
	signed_int_add_ovf_cert_2s(foo, bar);
	signed_int_add_ovf_cert_any(foo, bar);


	printf("\nTesting signed integer addition overflow detection....\n\n");

	SINT_INIT(INT_MAX, 1);

	signed_int_add_ovf_1(foo, bar);		/* INT_MAX + 1 */
	signed_int_add_ovf_2(foo, bar);
	signed_int_add_ovf_3(foo, bar);
	signed_int_add_ovf_mot(foo, bar);
	signed_int_add_ovf_cert_2s(foo, bar);
	signed_int_add_ovf_cert_any(foo, bar);

	SINT_INIT(INT_MAX, INT_MAX);

	signed_int_add_ovf_1(foo, bar);		/* INT_MAX + INT_MAX */
	signed_int_add_ovf_2(foo, bar);
	signed_int_add_ovf_3(foo, bar);
	signed_int_add_ovf_mot(foo, bar);
	signed_int_add_ovf_cert_2s(foo, bar);
	signed_int_add_ovf_cert_any(foo, bar);


	printf("\nTesting signed integer subtraction OK....\n\n");

	SINT_INIT(1, 1);

	signed_int_sub_undf_3(foo, bar);	/* 1 - 1 */
	signed_int_sub_undf_mot(foo, bar);

	SINT_INIT(0, 1);

	signed_int_sub_undf_3(foo, bar);	/* 0 - 1 */
	signed_int_sub_undf_mot(foo, bar);

	SINT_INIT((INT_MIN + 1), 1);

	signed_int_sub_undf_3(foo, bar);	/* (INT_MIN + 1) - 1 */
	signed_int_sub_undf_mot(foo, bar);


	printf("\nTesting signed integer subtraction underflow detection....\n\n");

	SINT_INIT(INT_MIN, 1);

	signed_int_sub_undf_3(foo, bar);	/* INT_MIN - 1 */
	signed_int_sub_undf_mot(foo, bar);


	SINT_INIT(INT_MIN, INT_MAX);

	signed_int_sub_undf_3(foo, bar);	/* INT_MIN - INT_MAX */
	signed_int_sub_undf_mot(foo, bar);


	/*
	 * finally some "sure to trap" trap checking!
	 *
	 * XXX For some reason 'clang -ftrapv -O3' will trigger a trap (using
	 * an illegal instruction on x86_64) and make it look (to gdb at least)
	 * that it occurred here even though it should have been way up on the
	 * first native signed integer INT_MAX+1 test above (which is where it
	 * appears to occur if optimization is disabled with '-O0').
	 */

	SINT_INIT(INT_MAX, 0);
	printf("\nsigned integer division by zero attempt: %d / %d\n", foo, bar);
	res = foo / bar;
	printf("*** signed integer division by zero: %d / %d = %d!\n", foo, bar, res);

#ifdef __GNUC__
	printf("\ncalling all traps...\n");
	__builtin_trap();
#endif

	exit(0);
}

void
unsigned_int_add_ovf(ufoo, ubar)
	unsigned int ufoo;
	unsigned int ubar;
{
	unsigned int usum;

	/*
	 * the simplest (and most portable) case is for addition of unsigned
	 * integers -- just check that the result is not less than either of
	 * the operands:
	 *
	 * (you should be using unsigned ints for array subscripts anyway!)
	 */
	usum = ufoo + ubar;
	if (usum < ufoo || usum < ubar) {
		printf("*** unsigned integer addition overflow: %u + %u = %u!\n", ufoo, ubar, usum);
	} else {
		printf("unsigned integer addition OK: %u + %u = %u\n", ufoo, ubar, usum);
	}
}

void
unsigned_int_sub_undf(ufoo, ubar)
	unsigned int ufoo;
	unsigned int ubar;
{
	unsigned int udiff;

	/*
	 * similarly for subtraction -- just check that the result is not
	 * greater than either of the operands:
	 */
	udiff = ufoo - ubar;
	if (udiff > ufoo || udiff > ubar) {
		printf("*** unsigned integer subtraction underflow: %u - %u = %u!\n", ufoo, ubar, udiff);
	} else {
		printf("unsigned integer subtraction OK: %u - %u = %u\n", ufoo, ubar, udiff);
	}
}

void
signed_int_add_ovf_1(foo, bar)
	signed int foo;
	signed int bar;
{
	signed int sum;

	/*
	 * For signed integers on two's-complement systems:
	 *
	 * When using operators such as '+' and '-', in which both operands
	 * have like signs, a change of sign in the result indicates an
	 * overflow condition.
	 *
	 * Note that if the signs of the operands are different then of course
	 * an overflow or underflow is impossible.
	 *
	 * XXX not GCC-4.4.x safe! (seems to work with GCC 4.6.1 on Mac OS X)
	 *
	 * XXX not safe on a compiler with working, and enabled, -ftrapv
	 */
	sum = foo + bar;
	if ((foo >= 0 && bar <= 0) ||
	    (foo <= 0 && bar >= 0)) {
		printf("signed 2's-complement integer addition 1 OK: %d + %d = %d\n", foo, bar, sum);
	} else if ((foo > 0 && sum < 0) ||
	           (foo < 0 && sum > 0)) {
		printf("*** signed 2's-complement integer addition overflow 1: %d + %d = %d!\n", foo, bar, sum);
	} else {
		printf("signed 2's-complement integer addition 1 OK: %d + %d = %d\n", foo, bar, sum);
	}
}

void
signed_int_add_ovf_2(foo, bar)
	signed int foo;
	signed int bar;
{
	signed int sum;

	/*
	 * perhaps even simpler than the sign-change check though for
	 * 2's-complement systems is this simple magnitude check of one operand
	 * against the result based on whether the other operand is positive or
	 * not (from GCC's "-ftrapv" runtime):
	 *
	 * NOTE:  This is probably the least expensive post-condition check!
	 *
	 * XXX not GCC 4.4.x safe!  (seems to work with GCC 4.6.1 on Mac OS X)
	 *
	 * XXX not safe on a compiler with working, and enabled, -ftrapv
	 */
	sum = foo + bar;
	if ((foo >= 0) ? (sum < bar) : (sum > bar)) {
		printf("*** signed 2's-complement integer addition overflow 2: %d + %d = %d!\n", foo, bar, sum);
	} else {
		printf("signed 2's-complement integer addition 2 OK: %d + %d = %d\n", foo, bar, sum);
#if ((__GNUC__ == 4) && (__GNUC_MINOR__ == 4))
		printf("XXX the above test may have failed!\n");
#endif
	}
}

void
signed_int_add_ovf_3(foo, bar)
	signed int foo;
	signed int bar;
{
	signed int sum;

	/*
	 * from Autoconf's manual, a tranformed variant of the above expression
	 *
	 * XXX not GCC 4.4.x safe! (seems to work with GCC 4.6.1 on Mac OS X)
	 *
	 * XXX not safe on a compiler with working, and enabled, -ftrapv
	 */
	sum = foo + bar;
	if ((sum < bar) != (foo < 0)) {
		printf("*** signed 2's-complement integer addition overflow 3: %d + %d = %d!\n", foo, bar, sum);
	} else {
		printf("signed 2's-complement integer addition 3 OK: %d + %d = %d\n", foo, bar, sum);
#if ((__GNUC__ == 4) && (__GNUC_MINOR__ == 4))
		printf("XXX the above test may have failed!\n");
#endif
	}
}

void
signed_int_add_ovf_mot(foo, bar)
	signed int foo;
	signed int bar;
{
	signed int sum;

	/*
	 * without regard to the sign of the operands we can also use the
	 * following on two's-complement systems (Apparently from Motorola's
	 * documented scheme for setting the overflow bit in MC68k family).
	 *
	 * XXX not safe on a compiler with working, and enabled, -ftrapv
	 */
	sum = foo + bar;
	if (((foo & bar & ~sum) | (~foo & ~bar & sum)) < 0) {
		printf("*** safe & simple signed 2's-complement integer addition overflow: %d + %d = %d!\n", foo, bar, sum);
	} else {
		printf("safe & simple signed signed 2's-complement integer addition OK: %d + %d = %d\n", foo, bar, sum);
	}
}

void
signed_int_add_ovf_cert_2s(foo, bar)
	signed int foo;
	signed int bar;
{
	signed int sum;

	/*
	 * The CERT secure coding page shows an even more complex version (I've
	 * replaced a complex static expression with INT_MIN) of this still
	 * very complex and much more expensive (especially on RISC CPUs)
	 * solution for testing for potential integer overflow when adding two
	 * integer values when the host uses a two's-complement representation
	 * for integers:
	 */
	sum = foo + bar;
	if (((foo ^ bar) | (((foo ^ (~(foo ^ bar) & INT_MIN)) + bar) ^ bar)) >= 0) {
		printf("*** expensive pre-condition signed 2's-complement integer addition overflow: %d + %d = %d!\n", foo, bar, sum);
	} else {
		printf("expensive pre-condition signed 2's-complement integer addition OK: %d + %d = %d\n", foo, bar, sum);
	}
}

void
signed_int_add_ovf_cert_any(foo, bar)
	signed int foo;
	signed int bar;
{
	signed int sum;

	/*
	 * The CERT secure coding page goes on to show another equally (or even
	 * more) expensive solution for testing for potential integer overflow
	 * when adding two integer values regardless of what binary
	 * representation the host uses for integers:
	 */
	sum = foo + bar;
	if ((foo > 0 && bar > 0 && foo > (INT_MAX - bar)) ||
	    (foo < 0 && bar < 0 && foo < (INT_MIN - bar))) {
		printf("*** expensive pre-conditioned signed integer addition overflow: %d + %d = %d!\n", foo, bar, sum);
	} else {
		printf("expensive pre-condition signed integer addition OK: %d + %d = %d\n", foo, bar, sum);
	}
}

void
signed_int_sub_undf_3(foo, bar)
	signed int foo;
	signed int bar;
{
	signed int diff;

	/*
	 * again from libgcc:
	 *
	 * NOTE:  This is probably the least expensive post-condition check!
	 *
	 * XXX not safe on a compiler with working, and enabled, -ftrapv
	 */
	diff = foo - bar;
	if ((bar >= 0) ? (diff > foo) : (diff < foo)) {
		printf("*** signed 2's-complement integer subtraction underflow 3: %d - %d = %d!\n", foo, bar, diff);
	} else {
		printf("signed 2's-complement integer subtraction OK 3: %d - %d = %d\n", foo, bar, diff);
	}
}

void
signed_int_sub_undf_mot(foo, bar)
	signed int foo;
	signed int bar;
{
	signed int diff;

	/*
	 * again from Motorola:
	 *
	 * XXX not safe on a compiler with working, and enabled, -ftrapv
	 */
	diff = foo - bar;
	if (((foo & ~bar & ~diff) | (~foo & bar & diff)) < 0) {
		printf("*** safe & simple signed 2's-complement integer subtraction overflow: %d - %d = %d!\n", foo, bar, diff);
	} else {
		printf("safe & simple signed signed 2's-complement integer subtraction OK: %d - %d = %d\n", foo, bar, diff);
	}
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm " fn "; " (default-value 'compile-command) " OPTIM='-O0 -ftrapv -fwrapv' " fn " && ./" fn))
 * End:
 */
