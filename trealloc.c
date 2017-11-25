/*
 * original posted:
 *
 * From: theo borm <theo4490@borm.org>
 * To: tech-kern@netbsd.org
 * Message-ID: <41A897B9.3080704@borm.org>
 * Date: Sat, 27 Nov 2004 16:05:29 +0100
 */

#include <stdio.h>
#include <stdlib.h>

int main(argc, argv)
	int argc;
	char *argv[];
{
	char *mem1;
	char *mem2;
	int i = 0;
	unsigned long int ulngint;
	size_t size;
	size_t increment;

	if (argc != 3) {
		fprintf(stderr, "Usage: realloctest <initial-size> <increment>\n");
		exit(1);
	}
	if (sscanf(argv[1], "%lu", &ulngint) != 1)
		errx(1, "Invalid size");
	size = ulngint;

	if (sscanf(argv[2], "%lu", &ulngint) != 1)
		errx(1, "Invalid increment");
	increment = ulngint;

	printf("first allocation (%lu bytes)... ", (unsigned long int) size);
	if ((mem1 = malloc(size)) == NULL)
		err(1, "malloc() failed");

	printf("succeeded.\n");
	fflush(stdout);

	while (1) {
		i++;
		size += increment;
		printf("step %d: reallocating %lu bytes... ", i, (unsigned long int) size);
		fflush(stdout);
		if ((mem2 = realloc(mem1, size)) == NULL) {
			warn("realloc() failed");
			free(mem1);
			exit(1);
		}
		printf("succeeded.\n");
		fflush(stdout);
		mem1 = mem2;
	}
	/* NOTREACHED */
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm " fn "; " (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
