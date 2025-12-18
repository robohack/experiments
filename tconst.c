#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * cdecl:
 *
 *	const char * const - a const pointer to const char
 *
 *	const char **	- a pointer to pointer to a const char (char values in read-only)
 *
 *	char * const *	- a pointer to const pointer to a char
 *
 *	char ** const	- a const pointer to pointer to a char
 *
 *	const char ** const	- a const pointer to pointer to a const char
 *
 *	const char * const * const	- a const pointer to a const pointer to a const char
 */

char * foo(char *);
const char * foo_1(const char *);
char * foo_2(const char *);
void foo_3(const char *, char const **);
void foo_4(const char *, char **);
char * foo_5(const char *);
char * foo_6(const char *);

char *foo_7(const char *, char * const []);
char *foo_8(const char *, const char * const []);
char *foo_9(const char *, char * const *);
char *foo_10(const char *, const char * const *);
const char *foo_11(const char *);

char *
foo(char *bar)
{
#if 0
	bar[4] = '*';		/* can cause UB if was passed a string constant */
#endif

	return bar;
}

const char *
foo_1(const char *bar)
{
	return bar;
}

char *
foo_2(const char *bar)
{
	return bar;	/* return discards const */
}

void
foo_3(const char *bar, const char **none)
{
	*none = bar;

	return;
}

void
foo_4(const char *bar, char **none)
{
	*none = bar;	/* assignment discards const */

	return;
}

char *
foo_5(const char *bar)
{
	return *((char **) &bar); /* cast discards const */
}

char *
foo_6(const char *bar)
{
	/* unconst */
	return (void *) (uintptr_t) (const void *) bar; /* OK!!! */
}

char *
foo_7(const char *bar, char * const none[])
{
	bar = none[0];
	printf("%s\n", bar);

	return none[0];
}

char *
foo_8(const char *bar, const char * const none[])
{
	bar = none[0];
	printf("%s\n", bar);

	return none[0];			/* discards qualifiers from pointer target type */
}

char *
foo_9(const char *bar, char * const *none)
{
	bar = *none;
	printf("%s\n", bar);

	return none[0];
}

char *
foo_10(const char *bar, const char * const *none)
{
	bar = *none;
	printf("%s\n", bar);

	return none[0];			/* discards qualifiers from pointer target type */
}

const char *
foo_11(const char *bar)
{
	bar = malloc((size_t) 100);	/* XXX XXX XXX NO ERROR OR WARNING!!! XXX XXX XXX */

	return bar;			/* XXX still no error, though here it is understandable */
}

int main(int, char *[]);

int
main(int argc __unused,
     char *argv[])
{
	char blah[100];
	const char cblah[100];
	const char *cbork = argv[0];
	char *bork = NULL;
	char * const borkc = NULL;
	char fooarray[] = "abc";	/* n.b.:  no warning, even with
					 * -Wwrite-strings, because it's just an
					 * initializer for a writable array
					 *
					 * N.B.B.:  This is the correct (but not
					 * necessarily most efficient) way to
					 * get a reference to a string
					 * "constant" that can be passed to a
					 * non-const-qualified parameter.
					 */
	printf("%s\n", fooarray);

	fooarray[0] = 'A';

	printf("%s\n", fooarray);

#ifdef CRASH_ME				/* make CPPFLAGS='-DCRASH_ME' tconst */
	bork = "def";			/* assignment discards ‘const’ qualifier from pointer target type */
	bork[0] = 'D';			/* n.b:  crashes with -Wwrite-strings, one of our defaults */
	printf("BORKED: %s\n", bork);
#endif

	bork = foo("a literal string constant\n"); /* xxx only with -Wwrite-strings:  discards qualifiers from pointer target type */
	printf(bork);			/* format is not a string constant (-Wformat-security) */

	cbork = foo_1("this is a literal too\n");

	bork = foo((const char *) "a literal string constant\n"); /* discards qualifiers from pointer target type */

	cbork = foo_1((const char *) "this is a literal too\n");

	cbork = blah;
	bork = cbork;		/* assignment discards const */
	*bork = '\0';

	cbork = cblah;
	bork = cbork;		/* assignment discards const */
	*bork = '\0';

	cbork = blah;
	/* unconst */
	bork = (void *) (uintptr_t) (const void *) cbork; /* OK */
	*bork = '\0';

	cbork = cblah;
	/* unconst */
	bork = (void *) (uintptr_t) (const void *) cbork; /* OK */
	*bork = '\0';

	cbork = foo_1(cblah);

	bork = foo_1(blah);	/* assignment discards const */
	*bork = '\0';

	bork = foo_2(blah);
	*bork = '\0';

	bork = foo_2(cblah);
	*bork = '\0';

	foo_3(blah, &cbork);
	*bork = '\0';
	foo_3(blah, &bork);	/* incompatible pointer type */
	*bork = '\0';
	foo_3(blah, &borkc);	/* incompatible pointer type */
	*bork = '\0';

	foo_3(blah, (const char **) &bork); /* xxx clang only: must have all intermediate pointers const qualified to be safe */
	*bork = '\0';
	foo_3(cbork, (char const **) &bork); /* silly form */ /* xxx clang only: must have all intermediate pointers const qualified to be safe */
	*bork = '\0';

	foo_3(bork, (char ** const) &bork);	/* incompatible pointer type */
	*bork = '\0';
	foo_3(bork, (char * const *) &bork);	/* incompatible pointer type */
	*bork = '\0';
	foo_3(bork, (char * const * const) &bork);	/* incompatible pointer type */
	*bork = '\0';
	foo_3(bork, (const char * const * const) &bork);/* discards qualifiers from pointer target type */
	*bork = '\0';

	foo_4(blah, &bork);
	*bork = '\0';

	bork = foo_5(blah);
	*bork = '\0';

	bork = foo_5(cblah);
	*bork = '\0';
	bork = foo_5(cbork);
	*bork = '\0';
	bork = foo_5(bork);
	*bork = '\0';

	bork = foo_6(cblah);
	*bork = '\0';
	bork = foo_6(blah);
	*bork = '\0';
	bork = foo_6(cbork);
	*bork = '\0';
	bork = foo_6(bork);
	*bork = '\0';

	foo_7(blah, argv);
	foo_8(blah, argv);		/* incompatible pointer type */
	foo_9(blah, argv);
	foo_10(blah, argv);		/* incompatible pointer type */

	cbork = foo_11(blah);

	exit(0);
	/* NOTREACHED */
}

/*
 * if not in CFLAGS already:  CPPFLAGS='-Wwrite-strings
 *
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm -f " fn "; " (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
