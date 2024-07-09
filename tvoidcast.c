#include <stdio.h>
#include <stdlib.h>

/*
 * Normally casting an expression to void discards its value, but GCC versions
 * 3.4 and newer have __attribute__((__warn_unused_result__)) which may cause
 * unwanted diagnostics in that case.
 *
 * Clang (currently) always honours the void cast.
 *
 * One instance of the "_wur" debate:
 *
 *	https://gcc.gnu.org/bugzilla/show_bug.cgi?id=66425
 *
 * Use __typeof__ and __extension__ to work around the problem, IFF the
 * workaround is known to be needed.
 */

#if defined(__GNUC__)
# if (__GNUC__ > 3) ||							\
	((__GNUC__ == 3) && (__GNUC_MINOR__ >= 4))
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
{			/* EXPECTED: warning: function declaration isn't a prototype */
}

/* splint clean... */
#define IGNORE(iexpr) if (1 == ((int) iexpr)) {;}
#define IGNOREP(pexpr) if (NULL == ((void *) pexpr)) {;}

int nv = 0;
int sv = 1;
int gignored __attribute__((unused));

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
	int isused;
	int notused;			/* EXPECTED:  warning: variable 'notused' set but not used */
	int lignored __attribute__((unused));
	int voided;
	int no_effect;

	(void) voided;
	no_effect; /* EXPECTED: warning: statement with no effect */

	bar(3); /* EXPECTED: warning: ignoring return value of 'bar', declared with attribute warn_unused_result */

	foo(4);
	foo(bar(5));
	isused = bar(6);

	printf("%d\n", isused);

	notused = bar(7);		/* use the result... */
//	(void) notused;			/* and ignore the set-but-not-used */

	gignored = bar(8);
	lignored = bar(9);

	(void) bar(10); /* GCC-stupidity: warning: ignoring return value of 'bar', declared with attribute warn_unused_result */

	ignoreit(bar(11)); /* clang: warning: too many arguments in call to 'ignoreit' */

	IGNORE(bar(12));

	ignore_value(bar(13));

	sv = bar(14);

	exit(0);
}


/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm -f " fn "; " (default-value 'compile-command) " CSTD='c99' " fn " && ./" fn))
 * End:
 */
