#include <stdio.h>
#ifdef __STDC__
# include <stdlib.h>
#endif

#ifdef __STDC__
int main(void);
#endif

/* based on:  https://stackoverflow.com/q/31115453/816536 */

int
main()
{
	printf("C%d\n", (int) (90 - (-4.5//* fooled you! */
				     -4.5)));
#ifndef __STDC__
	puts("Errr... actually K&R");
#endif
#if (__STDC_VERSION__ - 0) >= 201112L
	puts("Errr... actually C11");
#endif

	exit(0);
}

/* XXX compile with -std=c89 to write "C89", -std=c99 to write "C99" */

/* xxx in theory with a really old GCC one could use '-traditional' for K&R */

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm " fn "; " (default-value 'compile-command) " CPPFLAGS=-std=c89 " fn " && ./" fn))
 * End:
 */
