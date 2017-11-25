#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void);

int
main(void)
{
	unsigned int count = 2;
	char **ptr;
	char *res_storage[10];
	char **res = res_storage;

	printf("sizeof(void *) = %d\n\n", (int) sizeof(void *));

	printf("res     = %p\n", res);
	printf("res +% 2d = %p\n", count, res+count);
	printf("res[%d]  = %p\n", count, &res[count]);
	printf("storage = %p\n\n", &res_storage[count]);

	ptr = (char **) (void **) &res[count];
	printf("ptr     = %p\n", ptr);

	ptr = (char **) &res[count];
	printf("ptr     = %p\n", ptr);

	ptr = &res[count];
	printf("ptr     = %p\n", ptr);


	exit(0);
	/* NOTREACHED */
}


/*
 *  CPPFLAGS='-fstrict-aliasing -Wstrict-aliasing=1'
 *
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm " fn "; " (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
