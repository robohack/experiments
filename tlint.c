#include <stdio.h>
#include <string.h>

char *strcpy(char *, const char *);

void
bar()
{
	if (strcpy("bar", "foo"))
		strcpy("foo", "bar");
}

main()
{
	bar();
	exit(0);
	/* NOTREACHED */
}
