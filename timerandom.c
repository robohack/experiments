#include <stdio.h>

int
main(argc, argv)
	int argc;
	char *argv[];
{
	int i;

	for (i = 0; i < 10000000; i++)
		rand();

	printf("c: %d\n", i);
	exit(0);
}
