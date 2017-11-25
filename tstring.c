extern void nut(const char *, const char *, const char *);
extern void bar(char *, char *, char *);

extern int main(void);

char *g_fooptr = "abcdef";
char *g_barptr = "\
abcdef";
char g_fooarr2[] = "abcdef";
char g_fooarr1[] = { 'a', 'b', 'c', 'd', 'e', 'f', '\0' };

int
main()
{
	char *fooptr = "abcdef";
	char fooarr2[] = "abcdef";
	char fooarr1[] = { 'a', 'b', 'c', 'd', 'e', 'f', '\0' };
	static char *s_fooptr = "abcdef";
	static char s_fooarr2[] = "abcdef";
	static char s_fooarr1[] = { 'a', 'b', 'c', 'd', 'e', 'f', '\0' };

	nut(fooptr, fooarr1, fooarr2);

	bar(s_fooptr, s_fooarr1, s_fooarr2);

	fooptr = "\a\b\c\d\e\f\g\h\i\j\k\l\m\n\o\p\q\r\s\t\u\v\w\x\y\z\0\1\2\3\4\5\6\7\8\9\,\.\;\'\[\]\`\:\"\<\>\?\{\}\!\@\#\$\%\^\&\*\(\)\_\+";

	return 0;
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm " fn "; " (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
