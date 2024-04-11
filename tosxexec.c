#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void);

int
main(void)
{
	while (1) {
		printf("Hello world!\n");
		sleep(1);
	}
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm -f " fn "; " (default-value 'compile-command) " " fn))
 * End:
 */
