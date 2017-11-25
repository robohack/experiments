#include <stdio.h>
#if 0
# include "tcharptrarraydiff.h"
#else
void bar2(const char *, void *);
#endif

/*
 * Note that in part1 we'll declare this as:
 *
 *	extern char *foo;
 */
char foo[] = "12345678 array of chars called foo";
char ok[] = "array of chars caleld OK";
char *str = "a string";
char *strarr = "annother string";

extern int a;

int a;
int a;

void
bar2(const char *nm, void *f)
{
	printf("bar2: foo = %p\n", foo);
	printf("bar2: &foo = %p\n", &foo);
	printf("bar2: ok = %p\n", ok);
	printf("bar2: str = %p\n", str);
	printf("bar2: strarr = %p\n", strarr);
	printf("bar2: &strarr = %p\n", &strarr);
	printf("bar2: input: f (%s) = %p\n", nm, f);

	putchar('\n');
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm " fn "; " (default-value 'compile-command) " OPTIM=-O0 PROG=tcharptrarraydiff OBJS='tcharptrarraydiff-part1.o tcharptrarraydiff-part2.o' && ./tcharptrarraydiff"))
 * End:
 */
