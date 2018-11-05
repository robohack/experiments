#include <sys/stat.h>							\

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]);

int
main(int argc,
     char *argv[])
{
	const char *dirnm = (argc == 2) ? argv[1] : ".///foo///";

	if (mkdir(dirnm, 0777) != 0) {
		fprintf(stderr, "mkdir('%s'): %s\n", dirnm, strerror(errno));
	}

	exit(0);
}


/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm " fn "; " (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
