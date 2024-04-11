#include <stdio.h>
#include <stdlib.h>

int __sflags(const char *mode, int *optr);

int main(int, char *[]);

int
main(argc, argv)
	int argc;
	char *argv[];
{
	int r;
	int mode;

	(void) argc;

	r = __sflags("re+", &mode);
	printf("<fcntl.h>: 're+' =\t(0x%02x) 0x%x\n", r, mode);
	r = __sflags("we+", &mode);
	printf("<fcntl.h>: 'we+' =\t(0x%02x) 0x%x\n", r, mode);
	r = __sflags("re", &mode);
	printf("<fcntl.h>: 're' =\t(0x%02x) 0x%x\n", r, mode);

	if (fopen(argv[1], "r") == NULL)
		perror(argv[1] ? argv[1] : "(null)");

	exit(0);
}


/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm -f " fn "; " (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
