#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <util.h>


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

static void
test(int base)
{
	unsigned int i;
	char buf[BUFSIZ];

	for (i = 0; i < ((base == 0 || base == 1) ? 4 : 17); i++) {
		intmax_t v = ipow(base, i);

		humanize_number(buf, 9, (int64_t) v, "Num", HN_AUTOSCALE, HN_DIVISOR_1000);
		printf("ipow(%d, %2d) = %-9s (%" PRIi64 ")\n", base, i, buf, v);
	}
	putchar('\n');

	for (i = 0; i < ((base == 0 || base == 1) ? 4 : 17); i++) {
		intmax_t v = bipow(base, i);

		humanize_number(buf, 9, (int64_t) v, "Num", HN_AUTOSCALE, HN_DIVISOR_1000);
		printf("bipow(%d, %2d) = %-9s(%" PRIi64 ")\n", base, i, buf, v);
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
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm " fn "; " (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
