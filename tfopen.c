#include <stdio.h>

main(argc, argv)
	int argc;
	char *argv[];
{
	if (fopen(argv[1], "r") < 0)
		perror(argv[1]);

	exit(0);
}
