/*
 * idea and example from:
 *
 *	http://mina.naguib.ca/blog/2010/12/09/c-optional-and-name-based-arguments.html
 *
 * See also tc99namedparams.c
 */

#include <stdio.h>
#include <stdlib.h>

/* nba.h */

#ifndef _NBA_H
# define _NBA_H

struct _find_args {
	const char *name;
	unsigned int min_age;
	unsigned int max_age;
};

#define _FindArgs(...)	((struct _find_args){__VA_ARGS__})
#define NbaFind(...)	(_nba_find(_FindArgs(__VA_ARGS__)))

void _nba_find(struct _find_args fa);

#endif /* _NBA_H */

/* nba.c */

/*
 * The implementation of _nba_find() itself, for the purposes of demonstrating
 * the use of optional function "parameters":
 */
void
_nba_find(struct _find_args fa)
{

	printf("Search criteria:\n");

	if (fa.name != NULL) {
		printf("\tName: [%s]\n", fa.name);
	}
	if (fa.min_age > 0) {
		printf("\tMin age: [%d]\n", fa.min_age);
	}
	if (fa.max_age > 0) {
		printf("\tMax age: [%d]\n", fa.max_age);
	}
	/*
	 * here a database lookup could be done, etc...
	 */

	return;
}

/*
 * example.c
 *
#include <nba.h>
 */

int main(void);

/*
 * Finally here are a couple of example use cases.
 *
 * The caller uses the Find() macro (notice the capitalization) to provide one
 * or more parameters.
 */
int
main()
{
	const char *n;
	const char *names[] = {"Albert", "Jane", NULL};
	int i;

	NbaFind(
		.min_age = 16,
		.max_age = 20
	);

	for (i = 0; (n = names[i]); i++) {
		NbaFind(
			.name = n
		);
	}

	exit(0);
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm " fn "; " (default-value 'compile-command) " CPPFLAGS='-std=c99' " fn " && ./" fn))
 * End:
 */
