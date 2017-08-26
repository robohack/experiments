#include <stdio.h>
#ifdef __STDC__
# include <stdlib.h>
#endif

#ifdef __STDC__
int main(void);
#endif

int
main()
{
	printf("C%d\n",(int)(90-(-4.5//**/
	                         -4.5)));

	exit(0);
}

/* XXX compile with -std=c89 to write "C89", -std=c99 to write "C99" */

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm " fn "; " (default-value 'compile-command) " CPPFLAGS=-std=c89 " fn " && ./" fn))
 * End:
 */
