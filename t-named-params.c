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

	printf("Criteria:\n");

	/*
	 * testing against default initializers shows all params are optional
	 */
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

struct stuff {
	int x;
	int y;
	int z;
};

static void
print_stuff(struct stuff *sptr)
{
	printf("%p: x=%d, y=%d, z=%d\n", (void *) sptr, sptr->x, sptr->y, sptr->z);
}

/*
 * show that the storage for a struct literal is ephemeral and sometimes reused
 */
static void
stuff_storage(void)
{
	int i;
	struct stuff *a_of_p[10];
	struct stuff a_of_s[10];

	printf("Direct:    ");
	print_stuff(&(struct stuff) { .z = 9, .x = 8, .y = 7 });

	for (i = 0; i < 10; i++) {
		/*
		 * The storage for the struct literal used here may be reused
		 * every time the loop is executed, and may even be the same as
		 * the one above:
		 */
		a_of_p[i] = &(struct stuff) { .z = i, .y = 2, .x = 1};
		/*
		 * Here's the safe usage where we copy the literal into an auto
		 * allocated array of structs:
		 */
		a_of_s[i] = (struct stuff) { .z = i, .y = 2, .x = 1};
		printf("a_of_p[%d]: ", i);
		print_stuff(a_of_p[i]);
		printf("a_of_s[%d]: ", i);
		print_stuff(&a_of_s[i]);
	}

	putchar('\n');
	printf("Trying to print everything again:\n");
	printf("Direct2:   ");
	/*
	 * sometimes this literal uses the same storage as one of the ones
	 * above:
	 */
	print_stuff(&(struct stuff) { .z = 5, .y = 4, .x = 6 });
	for (i = 0; i < 10; i++) {
		printf("a_of_p[%d]: ", i);
		print_stuff(a_of_p[i]);
		printf("a_of_s[%d]: ", i);
		print_stuff(&a_of_s[i]);
	}
}

int main(void);

int
main()
{
	const char *n;
	const char *names[] = {"Albert", "Jane", NULL};
	int i;

	/*
	 * The caller uses the NbaFind() macro (notice the capitalization) to
	 * provide one or more parameters.
	 */
	NbaFind(
		.min_age = 16,
		.max_age = 20
	);

	for (i = 0; (n = names[i]); i++) {
		NbaFind(
			.name = n
		);
	}

	putchar('\n');
	stuff_storage();

	exit(0);
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm " fn "; " (default-value 'compile-command) " CSTD='c99' " fn " && ./" fn))
 * End:
 */
