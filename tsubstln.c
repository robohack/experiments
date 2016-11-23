#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

int main(int, char **);

const char *argv0 = "progname";

int
main(int argc, char *argv[])
{
	char *ln;
	char *after;
	char *pos;
	size_t nr;
	size_t slen;
	size_t rlen;

	argv0 = (argv0 = strrchr(argv[0], '/')) ? argv0 + 1 : argv[0];

	if (argc != 3) {
		printf("usage: %s search replace\n", argv0);
		exit(1);
	}
	slen = strlen(argv[1]);
	rlen = strlen(argv[2]);

	/*
	 * *BSD fgetln(3) will read any length line (up to EOF) for which
	 * storage can be successfully allocated, or else it will return NULL.
	 *
	 * The resulting array may either end with a newline, or if EOF was
	 * reached then no newline`, so we can assume EOF if there is no newline
	 *
	 * note:  POSIX (IEEE 1003.1-2008) now has a getline(3) function, but
	 * it has a somewhat different API...
	 *
	 *	char *ln = NULL;
	 *	size_t lnsz = 0;
	 *	ssize_t nr;
	 *
	 *	while ((nr = getline(&ln, &lnsz, stdin)) > 0) {
	 *		like fgetln(), ln is not a C string,
	 *		and the last line's last char (ln[nr - 1]) may not be '\n'
	 *	}
	 */
	while ((ln = fgetln(stdin, &nr)) != NULL) {
		char *nln = NULL;

                if (ln[nr - 1] == '\n') {
			nr--;
			ln[nr] = '\0';
		} else {
	                if (!feof(stdin)) {
		                perror("fgetln: something funny happened on the way to the farm");
	                }
			/*
			 * the very last line may not end in a newline, but we
			 * need room for a NUL to keep strstr() safe
			 */
			if ((nln = (char *) malloc(nr + 1)) == NULL) {
				perror("malloc()/realloc()");
				exit(1);
			}
			memcpy(nln, ln, nr);
			nln[nr] = '\0';
			ln = nln;
		}
		after = ln;
		while ((pos = strstr(after, argv[1])) != NULL) {
			if (fwrite(after, (size_t) 1, (size_t) (pos - after), stdout) != (size_t) (pos - after)) {
				perror("fwrite(stdout)");
				exit(1);
			}
			if (fwrite(argv[2], (size_t) 1, rlen, stdout) != rlen) {
				perror("fwrite(stdout)");
				exit(1);
			}
			nr -= (size_t) (pos - after) + slen;
			after = pos + slen;
		}
		if (nr > 0) {
			if (fwrite(after, (size_t) 1, nr, stdout) != nr) {
				perror("fwrite(stdout)");
				exit(1);
			}
		}
		/* note: don't add a newline if there never was one */
		if (!nln && putchar('\n') != '\n') {
			perror("putchar('\n')");
			exit(1);
		}
		/*
		 * note: we do will not need to free nln here as the loop will
		 * necessarily end now (EOF was reached, the next fgetln() will
		 * return NULL), and thus the program will exit....
		 */
	}
	if (!feof(stdin) || ferror(stdin)) { /* checking feof() here is pure unnecessary paranoia */
		perror("fgetln(stdin)");
		exit(1);
	}
	/*
	 * XXX the exit() will close and flush stdout, but indication of any
	 * write error will be lost....  usually that's good enough for unix :-)
	 */
	exit(0);
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
