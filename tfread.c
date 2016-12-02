#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int, char *[]);

int
main(argc, argv)
	int argc;
	char *argv[];
{
	const char *fn;
	FILE *fp;
	char buf[BUFSIZ];
	ssize_t nr;
	int res;

	if (argc == 1 || argv[1] == NULL) {
		fp = stdin;
		fn = "stdin";
	} else if (argc == 2) {
		fn = argv[1];
		if ((fp = fopen(fn, "r")) == NULL) {
			perror(fn);
			exit(1);
		}
	} else {
		fprintf(stderr, "Usage: %s [file]\n", argv[0]);
		exit(1);
	}
#if 1
	res = fcntl(fileno(fp), F_SETFL, O_NONBLOCK);
	if (res != 0) {
		fprintf(stderr, "%s: fcntl: %s\n", fn, strerror(errno));
		exit(1);
	}
#endif

	while ((nr = fread(buf, (size_t) 1, (size_t) 100, fp)) > 0) {
		if (nr < 100) {
			fprintf(stderr, "%s: read was %jd bytes short of %d%s)\n",
			        fn, (intmax_t) 100 - nr, 100, feof(fp) ? ", at EOF" : "");
		}
	}

	if (!feof(fp)) {
		fprintf(stderr, "%s: returned before EOF (read %jd of %d)\n", fn, (intmax_t) nr, 100);
	}
	if (ferror(fp)) {
		perror(fn);
		exit(1);
	}

	exit(0);
}
