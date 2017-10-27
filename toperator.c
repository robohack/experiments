#include <stdio.h>
#include <stdlib.h>

int
main(int argc,
     char *argv[])
{
	int bar;
	int *foo;

	bar = 10;
	foo = & bar;

	if (! argc & 0x010) {
		exit(1);
	}

	printf("foo = %u, bar = %u, bar * foo = %u\n",
	       (unsigned long) foo,
	       (unsigned long) bar,
	       (unsigned long) (bar * foo));

	exit(0);
}
