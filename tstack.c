#include <stdio.h>

int
main(argc, argv)
	int argc;
	char *argv[];
{
	fooit(argv);
	exit(0);
}

int
fooit(argv)
	char *argv[];
{
	int len;
	char notsobigbuf[20] = { 0 };

#ifdef SAFE
	if ((len = read(fileno(stdin), notsobigbuf, sizeof(notsobigbuf))) <= 0) {
		perror("read()");
		exit(1);
	}
	fprintf(stderr, "%s: got %d bytes: '%.*s'\n", argv[0], len, len, notsobigbuf);
#else
	len = 0;
	gets(notsobigbuf);
	fprintf(stderr, "%s: len says %d bytes\n", argv[0], len);
	len = strlen(notsobigbuf);
	fprintf(stderr, "%s: got %d bytes: '%.*s'\n", argv[0], len, len, notsobigbuf);
#endif

	return 0;
}
