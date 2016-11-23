#include <stdio.h>
#include <strings.h>
#include <stdlib.h>

const char *argv0 = "progname";

int main(int, char *[]);

static size_t strrpart(const char *, const char *);

/*
 * return longest partial match of `find' at the end of `str', if any
 */
static size_t
strrpart(const char *str,
	 const char *find)
{
	size_t slen = strlen(str);
	size_t flen = strlen(find);

	while (flen) {
		if (strncmp(str + (slen - flen - 1), find, flen) == 0) {
			return flen;
		}
		flen--;
	}

	return 0;
}

int
main(int argc,
     char *argv[])
{
	char buf[20 + 1];
	char *after;			/* postion of end of a match in buf */
	char *pos;			/* position of a match in buf */
	size_t nr;			/* number of chars read */
	size_t slen;			/* search string length */
	size_t rlen;			/* replacement string length */
	size_t nmatched;		/* number of matching bytes at end */

	argv0 = (argv0 = strrchr(argv[0], '/')) ? argv0 + 1 : argv[0];

	if (argc != 3) {
		printf("usage: %s search replace\n", argv0);
		exit(1);
	}
	slen = strlen(argv[1]);
	rlen = strlen(argv[2]);

	nmatched = 0;
	while ((nr = fread(buf, 1, sizeof(buf) - 1, stdin)) > 0) {
		buf[nr] = '\0';		/* make the buffer into a string */

		after = buf;

		if (nmatched > 0) {
			/*
			 * look for the rest of the match at the beginning of
			 * the buffer
			 */
			if (strncmp(after, argv[1] + nmatched, slen - nmatched) == 0) {
				/* do the substitution */
				if (fwrite(argv[2], 1, rlen, stdout) != rlen) {
					perror("fwrite(stdout)");
					exit(1);
				}
				after += slen - nmatched;
			} else {
				/* output equivalent to rest of previous buf */
				if (fwrite(argv[1], 1, nmatched, stdout) != rlen) {
					perror("fwrite(stdout)");
					exit(1);
				}
			}
			nmatched = 0;
		}

		while ((pos = strstr(after, argv[1])) != NULL)  {
			if (fwrite(after, 1, (size_t) (pos - after), stdout) != (size_t) (pos - after)) {
				perror("fwrite(stdout)");
				exit(1);
			}
			if (fwrite(argv[2], 1, rlen, stdout) != rlen) {
				perror("fwrite(stdout)");
				exit(1);
			}
			nr -= (pos - after) + slen;
			after = pos + slen;
		}
		if (nr > 0) {
			/* look for a partial match at the end of the buffer */
			if ((nmatched = strrpart(after, argv[1])) > 0) {
				/*
				 * we will write just up to nmatched bytes from the
				 * end of the buffer
				 */
				nr -= nmatched;
			}
			if (nr > 0 && fwrite(after, 1, nr, stdout) != nr) {
				perror("fwrite(stdout)");
				exit(1);
			}
		}
	}
	if (!feof(stdin) || ferror(stdin)) { /* checking feof() here is pure unnecessary paranoia */
		perror("fread(stdin)");
		exit(1);
	}
	/*
	 * XXX the exit() will close and flush stdout, but indication of any
	 * write error will be lost....  usually that's good enough for unix :-)
	 */
	exit(0);
}
