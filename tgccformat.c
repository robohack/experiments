#include <sys/cdefs.h>
#include <stdio.h>
#include <err.h>
#include <string.h>

int main __P((int, char *[]));

static int foo();			/* non-prototype warning */
static int frob __P((char *));

int
main(argc, argv)
	int argc;
	char *argv[];
{
	char *none;

	strcpy(NULL, NULL);		/* SILENT '0' coercion! */

	none = 0;			/* SILENT '0' coercion! */
	none = NULL;
	none = 1;			/* assignment coercion warning */

	foo(0);				/* SILENT '0' coercion! */
	foo(NULL);			/* SILENT '0' coercion! */
	foo(1);				/* ERROR: BUSTED! */


	frob(0);			/* SILENT '0' coercion! */
	frob(NULL);
	frob(1);			/* prototype coercion warning */

	err(1, (const char *) NULL);	/* stupid NULL format warning */
}

int
foo(bar)
	char *bar;
{
	return 0;
}


int
frob(char *bar)
{
	return 0;
}
