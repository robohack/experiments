#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Parse a string of colon-prefixed single-character(alphanumeric) "flags".
 *
 * Flags with integer values should be followed by one or more decimal digits.
 * Note that errors parsing numbers will cause the value to be set to 0.
 * (Leading spaces on number are also allowed by virtue of the use of atoi() for
 * parsing numbers (other than the '0' flagged one), but this should not be
 * relied upon.)
 *
 * Flags with string values can contain anything but a ':' character.  Strings
 * are returned in newly allocated memory that can be passed to free(3) to be
 * released.  The initial (current) pointer will be passed to free(3) if the
 * flag is encountered (so it must be valid or NULL).
 *
 * Flags with char values will use only the character following the flag
 * character and ignore any further characters.
 *
 * Flags with boolean values should be stand-alone single letters.
 * Any characters after the flag character will be ignored.
 *
 * A colon followed by a digit ("[0-9]") will set the first CFLAGS_INT value
 * with a flag character of '0', so one "default" integer flag can be indicated
 * with a '0' as the flag character.
 *
 * The last value seen wins (as in getopt(3)).
 *
 *	int othernum = 0;
 *	int firstnum = 0;
 *	char cflag = '\0';
 *	char *strptr = NULL;
 *	bool zflag = false;
 *	struct cflags_cfg cflags[] = {
 *		{ 'N', CFLAGS_INT, &othernum },
 *		{ '0', CFLAGS_INT, &firstnum },		// first ":[1-9]" flag
 *		{ 'c', CFLAGS_CHAR, &cflag },
 *		{ 's', CFLAGS_STR, &strptr },
 *		{ 'z', CFLAGS_BOOL, &zflag },
 *		{ 0 },
 *	};
 *	(void) cflags_parse(cflags, ":4:N9:cX:sfoobar:z:y:sXYZZY:c");
 */
typedef enum {
	CFLAGS_BOOL,
	CFLAGS_CHAR,
	CFLAGS_INT,
	CFLAGS_STR,
} cflags_t_e;

struct cflags_cfg {
	const char fl;			/* must not be [1-9] or ':' */
	cflags_t_e ft;
	void *fv;
};

bool cflags_parse(struct cflags_cfg cflags[], const char *str);

bool
cflags_parse(struct cflags_cfg cflags[],
             const char *str)
{
	const char *p;
	bool res = true;

	for (p = strchr(str, ':'); p != NULL; (p = strchr(p, ':'))) {
		unsigned int fi;

		res = false;
		p++;                    /* skip the ":" */
		for (fi = 0; cflags[fi].fl != '\0'; fi++) {
			if (cflags[fi].fl == p[0] && p[0] != '0') { /* ignore flag '0' */
				p++;    /* skip the flag character */
				switch (cflags[fi].ft) {
				case CFLAGS_INT:
					*((int *) cflags[fi].fv) = atoi(p);
					res = true;
					break;
				case CFLAGS_STR:
					free(*((char **) cflags[fi].fv));
					*((char **) cflags[fi].fv) = strndup(p, strcspn(p, ":"));
					res = true;
					break;
				case CFLAGS_BOOL:
					*((bool *) cflags[fi].fv) = true;
					res = true;
					break;
				case CFLAGS_CHAR:
					if (p[0] != ':') {
						*((char *) cflags[fi].fv) = p[0];
						res = true;
					}
					break;
				}
			}
		};
		/* handle ":[0-9]*" */
		if (! res && p[0] && strchr("0123456789", p[0]) != NULL) {
			for (fi = 0; cflags[fi].fl != '\0'; fi++) {
				if (cflags[fi].ft == CFLAGS_INT && cflags[fi].fl == '0') {
					*((int *) cflags[fi].fv) = atoi(p);
					res = true;
					break;
				}
			}
		}
	}

	return res;
}

int main(int, char *[]);

int
main(int argc,
     char *argv[])
{
	int othernum = 0;
	int firstnum = 0;
	char cflag = '\0';
	char *strptr = NULL;
	bool zflag = false;
	struct cflags_cfg cflags[] = {
		{ 'N', CFLAGS_INT, &othernum },
		{ '0', CFLAGS_INT, &firstnum },		// first ":[1-9]" flag
		{ 'c', CFLAGS_CHAR, &cflag },
		{ 's', CFLAGS_STR, &strptr },
		{ 'z', CFLAGS_BOOL, &zflag },
		{ 0 },
	};
	int i;

	for (i = 1; argc > 1; argc--) {
		bool res;

		res = cflags_parse(cflags, argv[i++]);

		printf("res = %s, othernum = %d, firstnum = %d, cflag = %c, strptr = %s, zflag = %d\n",
		       res ? "true" : "false", othernum, firstnum, cflag, strptr, zflag);
		if (strptr) {
			free(strptr);
			strptr = NULL;
		}
		othernum = 0;
		firstnum = 0;
		cflag = '\0';
		zflag = false;
	}

	exit(0);
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm " fn "; " (default-value 'compile-command) " " fn " && ./" fn " :4:N9:cX:sfoobar:z:y"))
 * End:
 */
