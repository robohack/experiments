#include <ctype.h>
#include <string.h>
#include <stdio.h>

extern void nut(const char *, const char *, const char *);
extern void bar(char *, char *, char *);

extern int main(void);

char *g_fooptr = "abcdef";
char *g_barptr = "\
abcdef";
char g_fooarr2[] = "abcdef";
char g_fooarr1[] = { 'a', 'b', 'c', 'd', 'e', 'f', '\0' };

void str2alnum(char *p);			/* destructive canonicalisation */

void
str2alnum(char *p)
{
	char *y;

	for (y = p; *p; p++) {
		if (isalnum(*p)) {
			*y++ = *p;
		}
	}
	if (y < (p + strlen(p) - 1)) {
		*y = '\0';
	}
	return;
}

void trim_trailing(char *str, int ch);

void
trim_trailing(char *str, int ch)
{
	size_t end;

	end = strlen(str) - 1;
	while (str[end] == ch) {
		str[end--] = '\0';
	}

	return;
}

int
main()
{
	char *fooptr = strdup("abcdef");
	char fooarr2[] = "abcdef";
	char fooarr1[] = { 'a', 'b', 'c', 'd', 'e', 'f', '\0' };
	static const char *s_fooptr = "abcdef";
	static char s_fooarr2[] = "abcdef";
	static char s_fooarr1[] = { 'a', 'b', 'c', 'd', 'e', 'f', '\0' };
	char trailing[] = "/var/bar/none/////";

#if 0
	nut(fooptr, fooarr1, fooarr2);

	bar(s_fooptr, s_fooarr1, s_fooarr2);
#endif

	/* n.b.:  \0 is not included, for obvious reasons! */
#ifdef INVALID_ESCAPES
	fooptr = strdup("\a\b\c\d\e\f\g\h\i\j\k\l\m\n\o\p\q\r\s\t\u\v\w\x\y\z\1\2\3\4\5\6\7\8\9\,\.\;\'\[\]\`\:\"\<\>\?\{\}\!\@\#\$\%\^\&\*\(\)\_\+");
#else
	fooptr = strdup("\a\b\c\d\e\f\g\h\i\j\k\l\m\n\o\p\q\r\s\t\v\w\y\z\1\2\3\4\5\6\7\8\9\,\.\;\'\[\]\`\:\"\<\>\?\{\}\!\@\#\$\%\^\&\*\(\)\_\+");
#endif

	trim_trailing(trailing, '/');
	printf("trimmed: %s.\n", trailing);

	str2alnum(fooptr);
	printf("only-alnum: %s.\n", fooptr);

#if 0
	bar(fooptr, fooarr1, fooarr2);
#endif

	return 0;
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name)))) (make (default-value 'compile-command))) (concat make " clean PROG="fn "; " make " " fn " && ./" fn))
 * End:
 */
