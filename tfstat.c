#include <sys/types.h>
#include <sys/stat.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>

int main(void);

int
main()
{
	struct stat sb;

#if 0
	if (revoke("revoked") < 0)
		err(1, "revoke(revoked)");
#endif
	if (fstat(0, &sb) < 0)
		err(1, "stdin");
	printf("stdin st_blksize = %ld\n", (long int) sb.st_blksize);
	if (fstat(1, &sb) < 0)
		err(1, "stdout");
	printf("stdout st_blksize = %ld\n", (long int) sb.st_blksize);
	if (fstat(2, &sb) < 0)
		err(1, "stderr");
	printf("stderr st_blksize = %ld\n", (long int) sb.st_blksize);
	exit(0);
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
