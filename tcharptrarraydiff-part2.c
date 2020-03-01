#include <sys/cdefs.h>
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
const char foo[] = "12345678 array of chars called foo";
char ok[] = "array of chars caleld OK";
const char *str = "a string";
const char *strarr = "annother string";
char const szarr[100] = "a short string in a long array";

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
	printf("bar2: szarr = %p\n", szarr);
	printf("bar2: &szarr = %p\n", &szarr);
	printf("bar2: input: f (%s) = %p\n", nm, f);

	putchar('\n');
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm tcharptrarraydiff tcharptrarraydiff-part*.o; " (default-value 'compile-command) " OPTIM=-O0 PROG=tcharptrarraydiff SRCS='tcharptrarraydiff-part1.c tcharptrarraydiff-part2.c' && ./tcharptrarraydiff"))
 * End:
 */
