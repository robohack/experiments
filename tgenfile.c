#include <stdio.h>
int
main(argc, argv)
	int argc;
	char *argv[];
{
	int i, j;
		if (argc != 3) {
		fprintf(stderr, "usage: %s count string\n", argv[0]);
		exit(2);
	}
	if ((i = atoi(argv[1])) <= 0) {
		fprintf(stderr, "%s: invalid count '%s'\n", argv[0], argv[1]);
		exit(2);
	}
	for (j = 1; i > 0; i--, j++) {
		printf("%d %s\n", j, argv[2]);
	}
	exit(0);
	/* NOTREACHED */
}
