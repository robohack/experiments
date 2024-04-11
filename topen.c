#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

int main(void);

int
main()
{
	if (open("foo.open", O_WRONLY | O_APPEND, 0644) < 0)
		perror("foo.open");

	exit(0);
}


/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm -f " fn "; " (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
