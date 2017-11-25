#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void);

#ifdef __APPLE__
/* not declared, but is valid as per <sys/syscall.h> since at least 10.6.8 */
extern int fdatasync(int);
#endif

int
main()
{
	if (fsync(1) < 0)
		err(1, "fsync(stdout)");
	if (fdatasync(1) < 0)
		err(1, "fdatasync(stdout)");
	exit(0);
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm " fn "; " (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
