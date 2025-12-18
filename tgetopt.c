#include <sys/param.h>

#include <assert.h>
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

const char *argv0 = "progname";		/* __BASE_FILE__ */

/*
 * note:  NetBSD has only had __NetBSD_Prereq__() in the trunk since 3.0, though
 * it was also pulled up by patching into netbsd-2-0 sometime during the RC
 * process (between netbsd-2-0-RC2 and netbsd-2-0-RC3).  Note also that netbsd-2
 * is spun off netbsd-2-0 at netbsd-2-0-RELEASE as it wasn't made beforehand.
 * In the early days the revision history branching is a bit twisty folks!
 */
#if (defined(__NetBSD_Prereq__) && __NetBSD_Prereq__(1, 6, 0)) ||	\
	(defined(__NetBSD_Version__) && __NetBSD_Version__ > 1060000000) || \
	defined(__minix) ||		/* xxx when? */			\
	(defined(__OpenBSD) && (OpenBSD > 201305)) ||			\
	(defined(__FreeBSD__) && (__FreeBSD_Version >= 0404000)) ||	\
	defined(__DragonFly__) ||	/* xxx when? */			\
	defined(__APPLE__)		/* xxx when? */
/* defined(__Darwin__) || */
# define HAVE_GETPROGNAME		/* defined */
#endif

#ifndef HAVE_GETPROGNAME
static const char *
getprogname(void)
{
	return argv0;
}
#endif

void usage(bool) __dead;
int main(int, char *[]);

void
usage(bool err)
{
	FILE *fp = err ? stdout : stderr;

	fprintf(fp, "Usage:  %s [-abc] [-p ival]\n", getprogname());
	fprintf(fp, "        %s [-l]\n", getprogname());
	if (! err)
		fprintf(fp, "\
	-[abc]	normal option flags\n\
	-l	a lone flag\n\
	-p ival	an integer option\n\
");

	exit(err ? EX_USAGE : EXIT_SUCCESS);
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
	long long int llval = 0;	/* default llval */

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
	 *
	 * So, as a result note that setprogname() cannot be used to add, e.g.,
	 * a sub-command to the name!
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

	if (optind != 1) {
		fprintf(stderr, "%s: optind has an unexpected value: %d (not 1)", getprogname(), optind);
		abort();
	}
	if (opterr != 1) {
		fprintf(stderr, "%s: opterr has an unexpected value: %d (not 1)", getprogname(), opterr);
		assert(opterr == 1);
		abort();
	}

	optind = 1;			/* start at the beginning (the default) */
#if 1
	opterr = 0;			/* getopt() will not print errors! (returns '?' with optopt set) */
#else
	opterr = 1;			/* the default: getopt() will print errors! (and never return '?') */
#endif
#if 0
# define MY_OPTSTR	":abcdefp:hlP:"	/* simulate a programming error, see below... */
#else
# define MY_OPTSTR	":abcp:hlP:"
#endif
	while ((ch = getopt(argc, argv, MY_OPTSTR)) != -1) {
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
		case 'P':
			llval = (int) e_opt_strtoll();
			break;
		case 'l':
			/* xxx should this just be an assert()? */
			if (argv[optind] &&
			    *argv[optind] == '-' &&
			    strchr(argv[optind], optopt)) {
				myflg = optind;
			} else {
				/* XXX wtf?  I don't remember why I did this! (looking for misbehaviour?) */
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
			exit(EXIT_SUCCESS);

		case 'h':
			usage(true);

		case '?':
			fprintf(stderr, "%s: unknown option flag: -%c\n", argv0, optopt);
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
	if (llval != 0) {
		printf("%s: -P: %lld\n", argv0, llval);
	}

	for (i = 0; i < argc; ++i) {
		printf("%s: parameter: '%s'\n", argv0, argv[i]);
	}

	exit(EXIT_SUCCESS);
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm -f " fn "; " (default-value 'compile-command) " " fn " && ./" fn " -?"))
 * End:
 */
