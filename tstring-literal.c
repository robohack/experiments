const char *gstring =
#ifdef UNIQUE
	"1 "
#endif
	"this is a literal string";
char garray[] =
#ifdef UNIQUE
	"2 "
#endif
	"this is a literal string"; /* that is just an initializer */

int
bar(int x,
    int y)
{
	const char *fstring =
#ifdef UNIQUE
		"3 "
#endif
		"this is a literal string";
	char farray[] =
#ifdef UNIQUE
		"4 "
#endif
		"this is a literal string";
	static const char *sfstring =
#ifdef UNIQUE
		"5 "
#endif
		"this is a literal string";
	static char sfarray[] =
#ifdef UNIQUE
		"6 "
#endif
		"this is a literal string"; /* that is just an initializer */

	switch (x) {
	case 0:
		return fstring[y];
	case 1:
		return farray[y];
	case 2:
		return sfstring[y];
	case 3:
		return sfarray[y];
	default:
		return farray[y];
	}
}

/*
 * add -DUNIQUE to force each string to be unique
 *
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "cc -O2 -S -c " fn ".c && cat " fn ".s"))
 * End:
 */
