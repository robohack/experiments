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
#else
	printf("__STDC__ = %ld\n", (long) __STDC__);
#endif

#ifdef __STDC_VERSION__
	printf("__STDC_VERSION__ = %ld\n", (long) __STDC_VERSION__);
# if (__STDC_VERSION__ - 0) >= 201112L && (__STDC_VERSION__ - 0) < 201700L
	puts("Errr... actually C11");
# elif (__STDC_VERSION__ - 0) >= 201700L && (__STDC_VERSION__ - 0) < 202000L
	puts("Errr... actually C17");
# elif (__STDC_VERSION__ - 0) >= 202000L
	puts("Errr... actually C2x");
# endif
#endif

#ifdef __GNUC__
	printf("__GNUC__ = %ld\n", (long) __GNUC__);
	puts("Unfortunately GCC always supports most of its extensions\n-- use '-pedantic' to get warnings when/where they are used");
	/*
	 * __STRICT_ANSI__ is really just a hint to system header files to allow
	 * them to avoid defining or declaring macros and/or symbols which are
	 * not compatible with ISO C.  As noted above, language extensions
	 * implemented by the compiler are always implemented, though warnings
	 * can be enabled to let you know if you've used something non-standard
	 * (though in the past there have been some bugs with these warnings not
	 * always appearing!).
	 */
# ifdef __STRICT_ANSI__
	puts("__STRICT_ANSI__ is defined, header files may omit some non-ISO-C symbols");
# endif
#else
# ifdef __STRICT_ANSI__
	puts("Nice!  __STRICT_ANSI__ is defined in a non-GCC compiler!!!!");
# endif
#endif

	exit(0);
}

/* XXX compile with -std=c89 to write "C89", -std=c99 to write "C99" */

/* xxx in theory with a really old GCC one could use '-traditional' for K&R */

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm " fn "; " (default-value 'compile-command) " CSTD='c11' " fn " && ./" fn))
 * End:
 */
