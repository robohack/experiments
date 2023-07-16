#include <stdio.h>
#include <stdlib.h>

static void __attribute__((constructor))
my_constructor(void)
{
	printf("%s: %d: %s\n", __FILE__, __LINE__, __FUNCTION__);
}

static void __attribute__((destructor))
my_destructor(void)
{
	printf("%s: %d: %s\n", __FILE__, __LINE__, __FUNCTION__);
}

int main(void);

int
main()
{
	printf("%s: %d: %s\n", __FILE__, __LINE__, __FUNCTION__);

	exit(0);
}


/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm " fn "; " (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
