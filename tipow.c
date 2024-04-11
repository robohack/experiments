#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <util.h>

#if (!defined(HAVE_STDBOOL_H) &&                                        \
     ((defined(__STDC_VERSION__) &&                                     \
       ((__STDC_VERSION__ - 0) >= 199901L)) ||                          \
      defined(_ISOC99_SOURCE) ||                                        \
      (defined(__NetBSD_Version__) &&                                   \
       ((__NetBSD_Version__ - 0) >= 106000000)) ||                      \
      (defined(__APPLE_CC__) && defined(__STDC__)) ||                   \
      defined(__linux__)))
# define HAVE_STDBOOL_H
#endif

#ifdef HAVE_STDBOOL_H
# include <stdbool.h>
#else /* !HAVE_STDBOOL_H  */
typedef enum bool_e { false = 0U, true = !false } bool;
#endif /* !HAVE_STDBOOL_H  */

static inline intmax_t
isq(intmax_t x)
{
	return x * x;
}

static intmax_t
ipow(int base,
     unsigned int expon)
{
	/* xxx some calculators report an error on 0**0, others say 1 */
	switch (base) {
	case 0:
		return expon == 0 ? 1 : 0;
	case 1:
		return 1;
	}
	switch (expon) {
	case 0:
		return 1;
	case 1:
		return base;
	}
	/*  */ {
		
#if 0
	intmax_t result = 1;

	/* brute force */
	for (result = base; expon > 1; expon--) {
		result = result * base;
	}

	return result;
#elif 1
	intmax_t temp;

	/* recursive exponentiation by squaring... */

	temp = isq(ipow(base, expon / 2));

	if (expon % 2 == 0)
		return temp;
	else
		return (base * temp);
#elif 0
	/* recursive exponentiation by squaring... */

	if (expon % 2 == 1) {
		return base * ipow(base, expon - 1);
	} else {
		return isq(ipow(base, expon / 2));
	}
#else
	intmax_t bbase = base;
	intmax_t result = 1;

	/* Exponentiation by squaring... */
	while (expon) {
		if (expon & 1)
			result *= bbase;
		expon >>= 1;
		bbase *= bbase;
	}

	return result;
#endif
		
	}
}

/*
 * bipow() = (x**y)-1
 *
 * I.e. for x==2, set bits 0 through y
 *
 * hmmm....  or for counting from zero!
 */
static intmax_t
bipow(int base,
      unsigned int expon)
{
	intmax_t result = ipow(base, expon);

	return ((result == 0) ? 0 : result - 1);
}

/*
 *
 * The binary representation of a power-of-two 2y is a 1 followed only by 0s.
 *
 * In such a case, x − 1 generates a binary number where the 1s turn into 0s and
 * the former 0s turn into 1s.
 *
 * For example, 8 = 1000b and 8 − 1 = 7 = 0111b.
 *
 * If x and x − 1 are binary ANDed then the result is only 0 if x is a power of
 * two (line 3).
 *
 * Restrictions:  The basic algorithm returns true for x = 0 and x = 1,
 * i.e. they are considered to be a power of two
 *
 * from:  bits.stephan-brumme.com
 */
static bool ispow2(uintmax_t x);

static bool
ispow2(uintmax_t x)
{
	/* xxx maybe zero shouldn't be a power of 2? */
	if (x == 0) {
		return false;
	}

	return ((x & (x - 1)) == 0);
}

static void
test(int base)
{
	unsigned int i;
	char buf[BUFSIZ];

	for (i = 0; i < ((base == 0 || base == 1) ? 4 : 17); i++) {
		intmax_t v = ipow(base, i);

		humanize_number(buf, (size_t) 9, (int64_t) v, "Num", HN_AUTOSCALE, HN_DIVISOR_1000);
		printf("ipow(%d, %2d) = %-9s (%jd)%s\n", base, i, buf, v, ispow2((uintmax_t) v) ? " [ispow2]" : "");
	}
	putchar('\n');

	for (i = 0; i < ((base == 0 || base == 1) ? 4 : 17); i++) {
		intmax_t v = bipow(base, i);

		humanize_number(buf, (size_t) 9, (int64_t) v, "Num", HN_AUTOSCALE, HN_DIVISOR_1000);
		printf("bipow(%d, %2d) = %-9s(%jd)%s\n", base, i, buf, v, ispow2((uintmax_t) v) ? " [ispow2]" : "");
	}
	putchar('\n');

	return;
}

int main(void);

int
main()
{
	test(0);
	test(1);
	test(2);
	test(10);
	test(42);

	exit(0);
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm -f " fn "; " (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
