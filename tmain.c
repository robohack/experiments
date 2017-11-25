/*
 * A "return X;" statement in main() is not precisely equivalent to a call to
 * "exit(X);", since the dynamic storage of main() vanishes when main()
 * returns.
 *
 * For example, the following program has undefined behavior:
 *
 * In fact on many systems running it will result in a SIGSEGV.
 *
 * On recent macOS (with recent Clang) the output is corrupted, and valgrind
 * warns the buffer being passed to write() in the final fflush() in exit() will
 * point to unaddressable storage.
 *
 * See template.c for a more complete program template.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void goodbye(void);

char *argv0 = "progname";

void
goodbye(void)
{
	printf("%s: Goodbye cruel world!\n", argv0);
}

#if defined(full_main)
int main(int, char *[], char *[]);	/* a prototype for main() must exist */
#else
# if defined(plain_main)
int main(int, char *[]);		/* but it need not declare unused params */
# else
int main(void);				/* or even any params */
# endif
#endif

int
#if defined(full_main)
main(int argc, char *argv[], char *envp[])
#else
# if defined(plain_main)
main(int argc, char *argv[])
# else
main()
# endif
#endif
{
	char buf[BUFSIZ];		/* "automatic" (dynamic) storage */

#if defined(full_main)
	(void) envp;
#endif
#if defined(full_main) || defined(plain_main)
	(void) argc;
	argv0 = (argv0 = strrchr(argv[0], '/')) ? argv0 + 1 : argv[0];
#else
	argv0 = "TMAIN";
#endif

	printf("%s: Hello, %s!\n\n", argv0, "world");

	/*
	 * here's the trick:  we use a local automatic variable as the buffer
	 * storage for stdout, and thus if !defined(CONFORMING) then the
	 * printf() call in goodbye() will try to use main()'s stack after
	 * main() has exited
	 */
	memset(buf, 0, sizeof(buf));
	setvbuf(stdout, buf, _IOFBF, sizeof(buf));

	/* a printf() call or other use of stdout here is OK */

	atexit(goodbye);	/* goodbye() will be called after return; */

	printf("%s: Hello, buffered %s!\n", argv0, "world");
	fputc('\n', stderr);

#if defined(CONFORMING)
	exit(0);
#else
	return 0;
#endif
}

/*
 * The program becomes strictly conforming if one replaces main()'s "return 0;"
 * statement with a call to "exit(0);".  (-DCONFORMING)
 */

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm " fn "; " (default-value 'compile-command) " CPPFLAGS='-Dfull_main -DCONFORMING' " fn " && ./" fn))
 * End:
 */
