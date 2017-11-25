#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * dynamic size_max calculations
 */

static char *
binary_fmt(unsigned long long int x,
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
		while (s > str) {
			*--s = '0';
		}
	}

	return s;
}

int	main(void);

int
main(void)
{
	unsigned long long int_max;
	unsigned long long ssize_max;
	ssize_t ssize_max_r;
	volatile unsigned int bits;
	volatile unsigned int char_bits;

#ifndef CHAR_BIT			/* xxx should be in <limits.h> */
# ifdef __CHAR_BIT__
#  define CHAR_BIT	__CHAR_BIT__
# else
#  define CHAR_BIT	8
# endif
#endif
	char_bits = 0;
	bits = 1;
	while ((~0U & (~0U >> (bits - 1))) != 1) {
		bits++;
	}
	if (bits % sizeof(int) == 0) {
		char_bits = bits / sizeof(int);
	}
	printf("char bits = %d %s CHAR_BIT [%d]\n", char_bits,
	       char_bits == CHAR_BIT ? "==" : "!=", (int) CHAR_BIT);

#define SSIZE_BITS	((sizeof(size_t) * CHAR_BIT) - 1) /* subtract one for sign bit */

	puts("by compliment:");

	ssize_max = ~((1LLU << SSIZE_BITS) - 1);
	ssize_max = (ssize_max & -ssize_max);
	ssize_max--;

	fprintf(stdout, "ssize_max[%s]\nSSIZE_MAX[%s]\n",
	        binary_fmt((unsigned long long) ssize_max, true),
	        binary_fmt((unsigned long long) SSIZE_MAX, true));
	if (ssize_max != SSIZE_MAX) {
		fprintf(stderr, "ssize_max[%llu] != SSIZE_MAX[%llu]\n",
		        (unsigned long long) ssize_max, (unsigned long long) SSIZE_MAX);
	}

	puts("by shift:");

	ssize_max = ~0LLU >> ((sizeof(0LLU) * CHAR_BIT) - SSIZE_BITS);

	fprintf(stdout, "ssize_max[%s]\nSSIZE_MAX[%s]\n",
		binary_fmt((unsigned long long) ssize_max, true),
		binary_fmt((unsigned long long) SSIZE_MAX, true));
	if (ssize_max != SSIZE_MAX) {
		fprintf(stderr, "ssize_max[%llu] != SSIZE_MAX[%llu]\n",
		        (unsigned long long) ssize_max, (unsigned long long) SSIZE_MAX);
	}

	puts("by cast:");

	ssize_max = (ssize_t) (((size_t) -1LL) >> 1);

	fprintf(stdout, "ssize_max[%s]\nSSIZE_MAX[%s]\n",
		binary_fmt((unsigned long long) ssize_max, true),
		binary_fmt((unsigned long long) SSIZE_MAX, true));
	if (ssize_max != SSIZE_MAX) {
		fprintf(stderr, "ssize_max[%llu] != SSIZE_MAX[%llu]\n",
		        (unsigned long long) ssize_max, (unsigned long long) SSIZE_MAX);
	}

	puts("by cast to fit:");

	ssize_max_r = (ssize_t) (((size_t) -1LL) >> 1);

	fprintf(stdout, "ssize_max[%s]\nSSIZE_MAX[%s]\n",
		binary_fmt((unsigned long long) ssize_max_r, true),
		binary_fmt((unsigned long long) SSIZE_MAX, true));
	if (ssize_max_r != SSIZE_MAX) {
		fprintf(stderr, "ssize_max[%llu] != SSIZE_MAX[%llu]\n",
		        (unsigned long long) ssize_max_r, (unsigned long long) SSIZE_MAX);
	}

	puts("by shift:");

#define INT_BITS	((sizeof(int) * CHAR_BIT) - 1) /* subtract one for sign bit */
	int_max = ~0LLU >> ((sizeof(0LLU) * CHAR_BIT) - INT_BITS);

	fprintf(stdout, "  int_max[%s]\n  INT_MAX[%s]\n",
		binary_fmt((unsigned long long) int_max, true),
		binary_fmt((unsigned long long) INT_MAX, true));
	if (int_max != INT_MAX) {
		fprintf(stderr, "  int_max[%llu] != INT_MAX[%llu]\n",
		        (unsigned long long) int_max, (unsigned long long) INT_MAX);
	}

	puts("by cast:");

	int_max = (signed int) (((unsigned int) -1LL) >> 1);

	fprintf(stdout, "  int_max[%s]\n  INT_MAX[%s]\n",
		binary_fmt((unsigned long long) int_max, true),
		binary_fmt((unsigned long long) INT_MAX, true));
	if (int_max != INT_MAX) {
		fprintf(stderr, "  int_max[%llu] != INT_MAX[%llu]\n",
		        (unsigned long long) int_max, (unsigned long long) INT_MAX);
	}

	exit(0);
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm " fn "; " (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
