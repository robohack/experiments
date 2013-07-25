#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

pid_t globalCount=0;

int main(void);

int
main()
{

	fork();
	fork() && fork() || fork();
	fork();

	fprintf(stderr, "%lu\n", (unsigned int) getpid());
	sleep(1);

	exit(0);
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat (default-value 'compile-command) " " fn " && ./" fn " | sort -u | wc -l"))
 * End:
 */
