#include <sys/cdefs.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <unistd.h>

void	usage(int) __dead;

/* The name of the program */
char	*progname =	"XXX";

int
main(int argc, char **argv)
{
	extern int optind;	/* Set by getopt(): index of first non-option
				 * argument in argv[] */
	extern char *optarg;	/* Set by getopt(): points to an option's
				 * argument */

	int opt;

	/*
	 * Set the progname from argv[0], but only if it is provided
	 *
	 * N.B.:  C99 5.1.2.2.1 requires that argv[0][0] is a NUL byte even if
	 * argc==0, but can we trust the system to be compliant?
	 */
	if (argc > 0 && argv[0] && *argv[0]) {
		/* strip any leading path, by looking for the last /. */
		progname = strrchr(argv[0], '/');
		if (progname)
			progname++;
		else
			progname = argv[0];
	}

	/* Parse any options: the XXX is a string of valid option letters;
	 * if an option takes an argument, the letter is followed by a
	 * colon (:).
	 */
	optind = 1;			/* start at the beginning */
	opterr = 0;			/* getopt() will not print errors, but
					 * will return '?' with optopt set to
					 * the unknown option */
	/* see tgetopt.c for more ideas */
	while ((opt = getopt(argc, argv, ":XXX")) != EOF) {
		switch (opt) {
		/* enter a case for each option letter, eg, 'x' */
		case XXX:
			break;

		case '?':
			fprintf(stderr, "%s: unknown option -%c\n", argv0, optopt);
			usage(false);

		case ':':
			/*
			 * NOTE:  a leading ':' in optstring causes getopt() to
			 * return a ':' when an option is missing its parameter.
			 */
			fprintf(stderr, "%s: missing parameter for -%c\n", argv0, optopt);
			usage(false);

		default:
			fprintf(stderr, "%s: programming error, unhandled flag: %c\n", progname, opt);
			abort();

		}
	}
	argc -= optind;
	argv += optind;

	for (i = 0; i < argc; ++i) {
		printf("%s: parameter: '%s'\n", argv0, argv[i]);
		XXX do something with each parameter
	}

	XXX put you main body of code here.

	exit(0);
}

void
usage(int verbose)
{
	/* Describe the options your program takes.  The first XXX should
	 * be the argument passed to getopt() above (without the colons).
	 * The second XXX describes the non-option arguments your program
	 * takes (if any).
	 */
	fprintf(stderr, "Usage: %s [-XXX] XXX\n", progname);
	if (verbose)
		/* A verbose description of what the options do */
		fprintf(stderr, "\
	-XXX	does something\n\
");

	exit(EX_USAGE);
}
