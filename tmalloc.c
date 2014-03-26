#include <sys/cdefs.h>
#include <err.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static size_t
vol_getter(volatile int n)		/* fool the optimizer.... */
{
	return SIZE_MAX / n;
}

int
main(void);	/* a prototype for main() must exist */

int
main()
{
	int i;
	int n = 0;
	char *z = (char *) malloc((size_t) 0);
	char *p = (char *) malloc((size_t) 1000000UL);
	size_t sz;
	size_t num = vol_getter(2);	/* too large -- should overflow */

	if (z) {
		printf("malloc(0) = %p\n", z);
		free(z);
	} else {
		printf("malloc(0) = NULL\n");
	}

	for (i = 0; i < 1000000; ++i) {
		if (*(p + i)) {
			++n;
		}
	}

	printf("%d nonzero bytes found\n", n);

	free(p);

	/*
	 * if the malloc() argument is the result of an expression, make sure
	 * to test for overflow first!
	 *
	 * See the implementation of calloc(3) for a standard overflow check:
	 *
	 *	if (size != 0 && (num * size) / size != num)
	 */
	if (num != 0 && num > SIZE_MAX / sizeof(i)) {
		errno = ENOMEM;
		err(1, "size_t calculation overflow");
	}

	sz = sizeof(i) * num;		/* allocate for "num" integers */
	p = malloc(sz);
	free(p);

	exit(0);
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
