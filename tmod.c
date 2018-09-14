#include <stdio.h>
#include <stdlib.h>

#if 0
int main(int, char *[]);
#else
int main(void);
#endif

int
#if 0
main(int argv __unused, char *argc[] __unused)
#else
main()
#endif
{
	printf("0 % 100 = %d\n", 0 % 100);

	exit(0);
}


/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm " fn "; " (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
