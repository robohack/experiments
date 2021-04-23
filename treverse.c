#include <sys/param.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <unistd.h>
#include <vis.h>

void strreverse(char *);
char *strrev(const char *);
char *sentence_rev(const char *);

/*
 * strreverse() -- reverse a string "in place".
 * 
 * Assume 'start' points to a valid, writable, NUL-terminated string.
 */
void
strreverse(char *start)
{
	char *end = start;

	while (*end != '\0')
		end++;
	/* note 'end' now points past the end of the string, at the NUL */
	while (start < end) {
		char tmpch = *start;

		*start++ = *--end;
		*end = tmpch;
	}

	return;
}

/*
 * strrev() -- return a reversed copy of a string.
 * 
 * Assume 'string' points to a valid, NUL-terminated string.
 *
 * Works without modifying the original.
 *
 * Returned value is a pointer to new storage that may be released when it is
 * no longer needed by passing it to free(2).
 *
 * Returns NULL if enough new storage cannot be allocated and with errno set as
 * per malloc(3).
 */
char *
strrev(const char *string)
{
	size_t len = 0;
	const char *end = string;
	char *new;
	char *newp;

	while (*end != '\0') {
		len++;
		end++;
	}
	/* note 'end' now points past the end of the string, at the NUL */

	if ((newp = new = malloc(len)) == NULL) {
		return NULL;
	}

	while (end > string) {
		*newp++ = *--end;
	}

	return new;
}


/*
 * sentence_rev() -- reverse the "words" of a "sentence" in place.
 * 
 * Assume 'start' points to a valid, NUL-terminated char string.
 *
 * Note:  this implementation reverses a copy of the whole string, then reverses
 * each word in place, but does not (yet) reverse the inter-word whitespace back
 * to it's previous orientation, nor does it try to put the whitespace groups
 * back into the position they once were.  E.g. if the first word was followed
 * by one tab and the last word was preceded by two spaces, now the first word
 * will now be followed by two spaces and the last word will be preceded by one
 * tab.
 *
 * Allowing tabs as word separators in the first place is probably bogus, but it
 * allows us to see that the inter-word whitespace character groups are each
 * reversed in order as well as the groups overall being reversed in their
 * placement between words.
 *
 * Fixing this would require using a rather less efficient algorithm that would
 * slide the middle of the string back or forth to accomodate different-length
 * words.
 *
 * longish middle shrt
 * songish middle lhrt
 * shngish middle lort
 * shrgish middle lont
 * shrtish middle long
 * shrtsh middle longi
 * shrth middle longis
 * shrt middle longish
 *
 * longish mid dle shrt
 * songish mid dle lhrt
 * shngish mid dle lort
 * shrgish mid dle lont
 * shrtish mid dle long
 * shrtsh mid dle longi
 * shrth mid dle longis
 * shrt mid dle longish
 * shrt did mle longish
 * shrt dld mie longish
 * shrt dle mid longish
 *
 * longish middle for nothing shrt
 * songish middle for nothing lhrt
 * shngish middle for nothing lort
 * shrgish middle for nothing lont
 * shrtish middle for nothing long
 * shrtsh middle for nothing longi
 * shrth middle for nothing longis
 * shrt middle for nothing longish
 * shrt niddle for mothing longish
 * shrt noddle for mithing longish
 * shrt notdle for midhing longish
 * shrt nothle for midding longish
 * shrt nothie for middlng longish
 * shrt nothin for middleg longish
 * shrt nothing for middle longish
 *
 */
char *
sentence_rev(const char *start)
{
	char *ns;
	char *ws;
	size_t wspn;
	char *we;

	ns = ws = strrev(start);
	wspn = strcspn(ns, " \t");
	do {
		char tch;

		we = ws + wspn;
		tch = *we;
		*we = '\0';
		strreverse(ws);
		*we = tch;
		ws = we + 1;
		ws += strspn(ws, " \t");
		wspn = strcspn(ws, " \t");
	} while (wspn && *we);

	return ns;
}

void usage(bool) __dead;
int main(int, char **);

extern char *optarg;	/* points to a ':' option's parameter */
extern int optind;	/* index of first non-option param in argv[] */
extern int optopt;	/* the currently parsed option character */
extern int opterr;	/* controls getopt()'s error handling */
extern int optreset;	/* tells getopt() to start over */

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

void
usage(bool err)
{
	FILE *fp = err ? stderr : stdout;

	fprintf(fp, "Usage:  %s [-s] string|sentence ...\n", getprogname());

	exit(err ? EX_USAGE : EXIT_SUCCESS);
}

int
main(int argc,
     char *argv[])
{
	int opt;
	int do_sentence = 0;

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

	optind = 1;			/* start at the beginning */
	opterr = 0;			/* getopt() will not print errors! */
	while ((opt = getopt(argc, argv, "hs")) != -1) {
		switch (opt) {
		case 's':
			do_sentence++;
			break;

		case 'h':
			usage(false);

		case '?':
			fprintf(stderr, "%s: unknown option flag -%c\n", getprogname(), optopt);
			usage(true);

		default:
			fprintf(stderr, "%s: programming error, unhandled flag: %c\n", getprogname(), opt);
			abort();

		}
	}
	argc -= optind;
	argv += optind;

	for (opt = 0; opt < argc; opt++) {
		char *rs;
		char *vs;

		vs = malloc(strlen(argv[opt]) * 4);
		strvis(vs, argv[opt], VIS_CSTYLE | VIS_TAB);

		printf("'%s' => ", vs);

		if (do_sentence) {
			rs = sentence_rev(argv[opt]);
		} else {
			rs = strrev(argv[opt]);
		}

		strvis(vs, rs, VIS_CSTYLE | VIS_TAB);
		printf("'%s'\n", vs);

		free(vs);
		free(rs);
	}

	exit(EXIT_SUCCESS);
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm " fn "; " (default-value 'compile-command) " " fn " && ./" fn " 'string to reverse'"))
 * End:
 */
