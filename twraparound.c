#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

int main(void);


int
main()
{
	unsigned int first;
	unsigned int second;

	first = UINT_MAX - 100;
	second = first + 200;

	printf("first = %u, second = %u, diff = %u\n", first, second, second - first);

	exit(0);
}


/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm " fn "; " (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
