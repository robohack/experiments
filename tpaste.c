#include <stdio.h>

int
main(argc, argv)
	int argc;
	char *argv[];
{
	int len;
	static char bigbuf[8192] = { 0 };

	if ((len = read(fileno(stdin), bigbuf, sizeof(bigbuf))) <= 0) {
		perror("read()");
		exit(1);
	}
	fprintf(stderr, "%s: got %ld bytes: %s", argv[0], (long) len, bigbuf);

	exit(0);
}
