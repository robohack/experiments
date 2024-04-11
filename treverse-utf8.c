/* -*- mode: c; coding: utf-8; -*- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * try these functions with something like this:
 *
 * ./treverse-utf8 'english' '¡Hola!' 'Grüß Gott' 'Hyvää päivää' 'Tere õhtust' 'Bonġu' 'Cześć!' 'Dobrý den' 'Здравствуйте!' 'Γειά σας' 'გამარჯობა'
 */


void str_reverse(char *start, char *end);
char *reverse_utf8char(char *start);
void reverse_utf8string(char *string);
void utf8rev(char *str);

/*
 * str_reverse() - reverse the bytes of a string, "in place"
 */
void
str_reverse(char *start,
	    char *end)			/* points at the last char in the
					 * string, before the NUL */
{
	while (start < end) {
		char tmpch = *start;

		*start++ = *end;
		*end-- = tmpch;
	}
}

/*
 * reverse_utf8char() - reverse the bytes of every UTF-8 char in a string (for
 * the purpose of undoing (or preparing for) str_reverse()'s effect on a string
 * containing UTF-8 encoded characters.
 */
char *
reverse_utf8char(char *start)
{
	char *end = start;

	/* look for the end of the next UTF8 character */
	while ((end[1] & 0xC0) == 0x80) {
		end++;
	}
	str_reverse(start, end);	/* reverse the bytes in the UTF8 char */

	return(end + 1);		/* return a pointer to the next char */
}

/* XXX and then there were "combining characters"....  hmmm.... */
void
reverse_utf8string(char *string)
{
	char *end = string;

	/* reverse the bytes of every UTF8 char in the string */
	while (*end) {
		end = reverse_utf8char(end);
	}
	/* reverse all of the bytes of the whole string */
	str_reverse(string, end - 1);
}

/*
 * a more complicated method
 */
void
utf8rev(char *str)
{
	/* this assumes that str is valid UTF-8 */
	char    *scanl, *scanr, *scanr2, c;

	/* first reverse the string */
	for (scanl= str, scanr= str + strlen(str); scanl < scanr;)
		c= *scanl, *scanl++ = *--scanr, *scanr = c;

	/* then scan all bytes and reverse each multibyte character */
	for (scanl = scanr = str; (c = *scanr++);) {
		if ((c & 0x80) == 0) /* ASCII char */
			scanl = scanr;
		else if ((c & 0xc0) == 0xc0) { /* start of multibyte */
			scanr2 = scanr;
			switch (scanr - scanl) {
			case 4:
				c = *scanl, *scanl++ = *--scanr, *scanr= c;
				/* FALLTHROUGH */
			case 3:
				/* FALLTHROUGH */
			case 2:
				c = *scanl, *scanl++ = *--scanr, *scanr = c;
			}
			scanr = scanl = scanr2;
		}
	}
}

char *argv0 = "progname";

int
main(int argc,
     char *argv[])
{
	char buffer[256];

	buffer[sizeof(buffer)-1]= '\0';	/* add a safety terminator */

	argv0 = (argv0 = strrchr(argv[0], '/')) ? argv0 + 1 : argv[0];

	printf("%s: beginning....\n", argv0);

	while (--argc > 0) {
		/* don't overwrite final safety NUL! */
		strncpy(buffer, argv[argc], sizeof(buffer)-1);

		printf("'%s' -> ", buffer);

		reverse_utf8string(buffer);
		printf("'%s'", buffer);

		/* don't overwrite final safety NUL! */
		strncpy(buffer, argv[argc], sizeof(buffer)-1);

		utf8rev(buffer);
		printf("; or -> '%s'\n", buffer);
	}
	printf("%s: Done.\n", argv0);

	exit(0);
	/* NOTREACHED */
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm -f " fn "; " (default-value 'compile-command) " " fn " && ./" fn " 'english' '¡Hola!' 'Grüß Gott' 'Hyvää päivää' 'Tere õhtust' 'Bonġu' 'Cześć!' 'Dobrý den' 'Здравствуйте!' 'Γειά σας' 'გამარჯობა'"))
 * End:
 */
