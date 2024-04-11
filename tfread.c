#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SMALL_BUFSIZ	100

int main(int, char *[]);

int
main(argc, argv)
	int argc;
	char *argv[];
{
	const char *fn;
	FILE *fp;
	char buf[SMALL_BUFSIZ + 1];
	size_t nr, tr = 0;

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
#if 0
	{
		int res;

		res = fcntl(fileno(fp), F_SETFL, O_NONBLOCK);
		if (res != 0) {
			fprintf(stderr, "%s: fcntl: %s\n", fn, strerror(errno));
			exit(1);
		}
	}
#endif

	/*
	 * n.b.:  we want to read chars (values of one byte), up to the size of
	 * our buffer (less one for the NUL) -- fread() returns the number of
	 * items read (nmemb), not the number of bytes read (though since our
	 * size of item is 1, this of course is equivalent, which is what we
	 * normally want), so we set the 'size' argument to be 1, and 'nmemb' to
	 * be the size of our buffer (or the number of bytes we want to read).
	 *
	 * Indeed for fread() a 'size' of other than 1 would be highly irregular
	 * for any code dealing with standard I/O streams.  It would only make
	 * sense when reading binary objects, be they basic types, or aggregate
	 * types.
	 */
	for (;;) {
		nr = fread(buf, (size_t) 1, sizeof(buf) - 1, fp);
		if (nr == 0) {
			/*
			 * Either of these tests alone should be sufficient to
			 * detect any error, but having both may suss out some
			 * extra details, depending on the <stdio>
			 */
			if (ferror(fp)) {
				fprintf(stderr, "%s: ferror: %s\n", fn, strerror(errno));
				exit(1);
			} else if (!feof(fp)) {
				/* returned zero before EOF */
				fprintf(stderr, "%s: fread: %s\n", fn, strerror(errno));
				exit(1);
			} /* else */
			fprintf(stderr, "%s: at EOF\n", fn);
			break;
		}
		buf[nr] = '\0';		/* make sure data is a valid C string */
		tr += nr;

		if (nr < SMALL_BUFSIZ) {
			fprintf(stderr, "%s: read was %ju bytes short of %d\n",
			        fn, (uintmax_t) SMALL_BUFSIZ - nr, SMALL_BUFSIZ);
		}
	}

	fprintf(stderr, "%s: total read: %ju\n", fn, (uintmax_t) tr);

	/*
	 * xxx it is not necessary to check these again after the loop as the
	 * only way out of the loop is for fread() to have returned zero, and in
	 * that case the inner test is sufficient.
	 */
	if (!feof(fp)) {
		fprintf(stderr, "%s: returned zero before EOF (read %jd of %d)\n",
			fn, (intmax_t) nr, SMALL_BUFSIZ);
	}
	if (ferror(fp)) {
		perror(fn);
		exit(1);
	}

	exit(0);
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm -f " fn "; " (default-value 'compile-command) " " fn " && echo 'hello, reader!' | ./" fn))
 * End:
 */
