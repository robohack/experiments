#include <err.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#ifndef EXIT_USAGE
# define EXIT_USAGE	2
#endif

const char *argv0 = "progname";

int
main(int argc, char *argv[], char *envp[] __unused)
{
	struct stat sb;
	int res;

	argv0 = (argv0 = strrchr(argv[0], '/')) ? argv0 + 1 : argv[0];

	if (argc != 2)
		errx(EXIT_USAGE, "usage: %s name", argv0);

	res = lstat(argv[1], &sb);
	if (res != 0)
		err(EXIT_FAILURE, "%s", argv[1]);
	res = unlink(argv[1]);
	if (res != 0)
		err(EXIT_FAILURE, "unlink(%s)", argv[1]);

	exit(0);
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm " fn "; " (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
