#include <sys/cdefs.h>

/* Linux doesn't have such complete portability support */
#ifndef __dead
# ifdef __GNUC_PREREQ__
#  if __GNUC_PREREQ__(2, 5)
#   define __dead	__attribute__((__noreturn__))
#  endif
# endif
#endif
#ifndef __dead
# define __dead	/* nothing (a comment for lint, but CPP ignores comments) */
#endif

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <unistd.h>

static void	usage(int) __dead;

/* The name of the program */
const char	*argv0 = "trename";
bool		verbose = false;

int
main(int argc, char **argv)
{
	extern int optind;	/* Set by getopt(): index of first non-option
				 * argument in argv[] */
	extern char *optarg;	/* Set by getopt(): points to an option's
				 * argument */
	int opt;
	int res;

	/* Set the progname from argv[0] */
	argv0 = (argv0 = strrchr(argv[0], '/')) ? argv0 + 1 : argv[0];

	optind = 1;			/* start at the beginning */
	opterr = 0;			/* getopt() will not print errors! */
	while ((opt = getopt(argc, argv, ":hv")) != EOF) {
		switch (opt) {
		/* enter a case for each option letter, eg, 'x' */
		case 'v':
			verbose = true;
			break;

		case 'h':
			usage(true);

		case '?':
			fprintf(stderr, "%s: unknown option -%c\n", argv0, optopt);
			usage(false);

		case ':':
			fprintf(stderr, "%s: missing parameter for -%c\n", argv0, optopt);
			usage(false);

		default:
			fprintf(stderr, "%s: programming error, unhandled flag: '%c'(%04o)\n",
				argv0, opt, (unsigned int) opt);
			abort();
		}
	}

	argc -= optind;
	argv += optind;

	if (argc != 2) {
		fprintf(stderr, "%s: requires exactly two parameters\n", argv0);
		usage(false);
	}
	res = rename(argv[0], argv[1]);
	if (res != 0) {
		perror("rename()");
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}

static void
usage(int help)
{
	fprintf(stderr, "Usage: %s [-v] old new\n", argv0);
	if (help)
		fprintf(stderr, "\
	-v	verbose!\n\
");

	exit(EX_USAGE);
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm " fn "; " (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
