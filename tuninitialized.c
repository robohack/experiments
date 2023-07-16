#include <stdio.h>
#include <stdlib.h>

int main(void);

int flurb;


static void
printit(void)
{
	int blaha;

	printf("blaha: %d, flurb: %d\n", blaha, flurb);
}

int
main()
{
	printf("Hello world!\n");
	printit();

	exit(0);
}


/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm " fn "; " (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
