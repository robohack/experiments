#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <vis.h>

int main(void);

int
main()
{
	char foo[100];

	strvis(foo, "\\\"\abcdef", VIS_CSTYLE);
	puts(foo);

	strvis(foo, "this is percent(%)F:  %F", VIS_TAB|VIS_NL);
	puts(foo);

	strvis(foo, "date > $HOME/tmp/$(TZ=UTC0 date +\%FT\%TZ).date", VIS_TAB|VIS_NL);
	puts(foo);

	exit(0);
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm -f " fn "; " (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
