#include <stdio.h>
#include <stdlib.h>

/*
 * Normally casting an expression to void discards its value, but GCC versions
 * 3.4 and newer have __attribute__((__warn_unused_result__)) which may cause
 * unwanted diagnostics in that case.
 *
 * Use __typeof__ and __extension__ to work around the problem, if the
 * workaround is known to be needed.
 */

#if defined(__GNUC__)
# if 3 < __GNUC__ + (4 < __GNUC_MINOR__)
#  define ignore_value(x)	(__extension__({__typeof__(x) __x = (x); (void) __x; }))
# endif
#endif
#ifndef ignore_value
# define ignore_value(x) ((void) (x))
#endif

void foo(int);

void
foo(int a)
{
	(void) a;

	/* xxx GCC only? violates the first constraint in C99 6.8.6.4 */

	return (void)0;
}

/* warn_unused_result: GCC 3.4 and newer (ignored by clang?) */

int bar(int)  __attribute__((warn_unused_result));

static inline void
ignoreit()		/* no parameter list allows any type parameter, but... */
{			/* warning: function declaration isn't a prototype */
}

/* splint clean... */
#define IGNORE(iexpr) if (1 == ((int) iexpr)) {;}
#define IGNOREP(pexpr) if (NULL == ((void *) pexpr)) {;}

int nv = 0;
int sv = 1;

int
bar(int p)
{
	foo(3);

	return (2 + p);
}

int main(void);

int
main()
{
	int boo;
	int none;

	(void) nv;

	foo(3);
	foo(bar(2));
	boo = bar(4);

	printf("%d\n", boo);

	none = bar(5);			/* use the result... */
//	(void) none;			/* and ignore the set-but-not-used */

	(void) bar(9); /* warning: ignoring return value of 'bar', declared with attribute warn_unused_result */

	ignoreit(bar(10)); /* clang: warning: too many arguments in call to 'ignoreit' */

	IGNORE(bar(8));

	ignore_value(bar(13));

	sv = bar(12);

	exit(0);
}


/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm " fn "; " (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
