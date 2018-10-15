#include <math.h>
#include <stdio.h>
#include <stdlib.h>

int
main(int argc,
     char *argv[])
{
	double x;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <x>\n", argv[0]);
		exit(1);
		/* NOTREACHED */
	}

	x = strtod(argv[1], NULL);

	printf("sin(%e) = %e\n", x, sin(x));

	exit(0);
	/* NOTREACHED */
}


/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm " fn "; " (default-value 'compile-command) " LDLIBS=-lm " fn " && ./" fn "-1.0974e-18"))
 * End:
 */
