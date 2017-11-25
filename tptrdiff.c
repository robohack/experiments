#include <assert.h>
#include <inttypes.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

int	main(void);

struct two_memb {
	char one;
	char *two;
};


ptrdiff_t member_direct(struct two_memb *s);

ptrdiff_t member_diff(struct two_memb *s);

ptrdiff_t
member_direct(struct two_memb *s)
{
	assert(s == NULL);
	return (ptrdiff_t) &s->two;
}

ptrdiff_t
member_diff(struct two_memb *s)
{
	return (char *)(&s->two) - (char *)s;
}


int
main(void)
{
	struct two_memb *foo;

	printf("offset: %lu\n", ((size_t)(unsigned long)(&((struct two_memb *)0)->two)));

	printf("subtract: %lu\n", ((size_t)(unsigned long)((char *)&((struct two_memb *)0)->two - (char *)0)));

	foo = NULL;

	printf("direct: %ju\n", member_direct(foo));

	printf("diff: %ld\n", member_diff(foo));

	exit(0);
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm " fn "; " (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
