#include <sys/cdefs.h>

#include <sys/param.h>

#include <err.h>
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <unistd.h>

extern char *optarg;	/* points to a ':' option's parameter */
extern int optind;	/* index of first non-option param in argv[] */
extern int optopt;	/* the currently parsed option character */
extern int opterr;	/* controls getopt()'s error handling */
extern int optreset;	/* tells getopt() to start over */

long int e_opt_strtol(void);

/*
 * extremely pedantic parameter evaluation
 */
long int
e_opt_strtol()
{
	long int ltmp;
	char *ep;

#if 0
	/* this one not normally needed with modern getopt(3) */
	if (optarg[0] == '\0') {
		err(EX_USAGE,
		    "an empty parameter value for '-%c' is not permitted\n",
		    optopt);
	}
#endif
	errno = 0;
	ltmp = strtol(optarg, &ep, 0);
	if (ep == optarg) {
		err(EX_USAGE,
		    "-%c param of '%s' is not a valid number",
		    optopt,
		    optarg);
	}
	if (*ep) {
		err(EX_USAGE,
		    "-%c param of '%s' has unsupported trailing unit specification characters",
		    optopt,
		    optarg);
	}
	if (errno != 0) {
		err(EX_USAGE,
		    "-%c param of '%s' is not convertible: %s",
		    optopt,
		    optarg,
		    strerror(errno));
	}
	/* xxx if sizeof(int) == sizeof(long int), the compiler may consider it dead code */
	if (ltmp > INT_MAX) {
		err(EX_USAGE,
		    "-%c param of %ld is too large (must be <= %d)",
		    optopt,
		    ltmp,
		    INT_MAX);
	}
	if (ltmp < 1) {
		err(EX_USAGE,
		    "-%c param of %ld is too small (must be > 0)",
		    optopt,
		    ltmp);
	}

	return ltmp;
}

long long int e_opt_strtoll(void);

/*
 * extremely pedantic parameter evaluation
 */
long long int
e_opt_strtoll()
{
	long long int lltmp;
	char *ep;

#if 0
	/* this one not normally needed with modern getopt(3) */
	if (optarg[0] == '\0') {
		err(EX_USAGE,
		    "an empty parameter value for '-%c' is not permitted\n",
		    optopt);
	}
#endif
	errno = 0;
	lltmp = strtoll(optarg, &ep, 0);
	if (ep == optarg) {
		err(EX_USAGE,
		    "-%c param of '%s' is not a valid number",
		    optopt,
		    optarg);
	}
	if (*ep) {
		err(EX_USAGE,
		    "-%c param of '%s' has unsupported trailing unit specification characters",
		    optopt,
		    optarg);
	}
	if (errno != 0) {
		err(EX_USAGE,
		    "-%c param of '%s' is not convertible: %s",
		    optopt,
		    optarg,
		    strerror(errno));
	}
	if (lltmp > LONG_MAX) {
		err(EX_USAGE,
		    "-%c param of %lld is too large (must be <= %d)",
		    optopt,
		    lltmp,
		    INT_MAX);
	}
	if (lltmp < 1) {
		err(EX_USAGE,
		    "-%c param of %lld is too small (must be > 0)",
		    optopt,
		    lltmp);
	}

	return lltmp;
}

const char *argv0 = "progname";

/*
 * note:  NetBSD has only had __NetBSD_Prereq__() in the trunk since 3.0, though
 * it was also pulled up by patching into netbsd-2-0 sometime during the RC
 * process (between netbsd-2-0-RC2 and netbsd-2-0-RC3).  Note also that netbsd-2
 * is spun off netbsd-2-0 at netbsd-2-0-RELEASE as it wasn't made beforehand.
 * In the early days the revision history branching is a bit twisty folks!
 */
#if (defined(__NetBSD_Prereq__) && __NetBSD_Prereq__(1, 6, 0)) ||	\
	(defined(__NetBSD_Version__) && __NetBSD_Version__ > 1060000000) || \
	defined(__minix) ||	/* xxx when? */				\
	(defined(__OpenBSD) && (OpenBSD > 201311)) ||			\
	defined(__FreeBSD__) ||						\
	defined(__DragonFly__) ||					\
	defined(__APPLE__)
# define HAVE_GETPROGNAME		/* defined */
#endif

#ifndef HAVE_GETPROGNAME
static const char *
getprogname(void)
{
	return argv0;
}
#endif

void usage(int) __dead;
int main(int, char *[]);

void
usage(int verbose)
{
	fprintf(stderr, "Usage:  %s [-abc] [-p ival]\n", argv0);
	fprintf(stderr, "        %s [-l]\n", argv0);
	if (verbose)
		fprintf(stderr, "\
	-[abc]	normal option flags\n\
	-l	a lone flag\n\
	-p ival	an integer option\n\
");

	exit(EX_USAGE);
}

int
main(argc, argv)
	int argc;
	char *argv[];
{
	int ch;
	int i;
	int myflg;
	int ival = 0;			/* default ival */

	argv0 = (argv0 = strrchr(argv[0], '/')) ? argv0 + 1 : argv[0];

#ifdef HAVE_GETPROGNAME			/* assume setprogname() too... */
	/*
	 * xxx getprogname() should be preceded by setprogname(argv[0]) but
	 * this is pointless and stupid in any hosted environment (even C99
	 * 5.1.2.2.1 guarantees that argv[0][0] is a NUL byte even if argc==0)
	 *
	 * In NetBSD 1.6 (and since), when these first appeared, setprogname()
	 * has had no effect as it is called by the C startup before main() is
	 * called, and it cannot be overridden by setprogname(), since that
	 * function does nothing at all.
	 */
	setprogname(argv0);
#endif


#if 0
	/* to support "sub-commands" */
	if (argc >= 2) {
		argc--;
		cmd = argv[1];
		argv[1] = argv[0];
		argv++;
	}
#endif

	optind = 1;			/* start at the beginning */
#if 1
	opterr = 0;			/* getopt() will not print errors! (returns '?' with optopt set) */
#else
	opterr = 1;			/* getopt() will print errors! (and never return '?') */
#endif
	while ((ch = getopt(argc, argv, ":abcp:hl")) != -1) {
		switch (ch) {
		case 'a':
		case 'b':
		case 'c':
			/*
			 * note we don't handle '-def' to simulate a
			 * programming error where those options are added to
			 * the getopt() 'optstring' parameter but not to the
			 * switch statement.
			 */
			printf("%s: normal handling for flag: %c\n", getprogname(), ch);
			break;
		case 'p':
			ival = (int) e_opt_strtol();
			break;
		case 'l':
			if (argv[optind] &&
			    *argv[optind] == '-' &&
			    strchr(argv[optind], optopt)) {
				myflg = optind;
			} else {
				/* XXX wtf?  I don't remember why I did this! */
				myflg = optind - 1;
			}
			/*
			 * if there are any other parameters, or this one is
			 * more than just one flag then there's a syntax error
			 */
			if (argc != 2 || strlen(argv[myflg]) > 2) {
				usage(false);
			}
			printf("%s: special standalone flag %c at myflg=%d: '%s'\n", getprogname(), optopt, myflg, argv[myflg]);
			exit(0);

		case 'h':
			usage(true);

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
			fprintf(stderr, "%s: programming error, unhandled flag: %c\n", getprogname(), ch);
			abort();

		}
	}
	argc -= optind;
	argv += optind;

	if (ival != 0) {
		printf("%s: -p: %d\n", argv0, ival);
	}

	for (i = 0; i < argc; ++i) {
		printf("%s: parameter: '%s'\n", argv0, argv[i]);
	}

	exit(0);
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm " fn "; " (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
