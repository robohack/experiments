#include <stdio.h>

char *foo = "This is a backslash continued string with leading \
             whitespace.";

main()
{
	printf("%s\n", foo);
	exit(0);
}
