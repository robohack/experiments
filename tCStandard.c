#include <stdlib.h>

void CStandard(void);

int
main(void)
{
	CStandard();

	exit(0);
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm " fn "; " (default-value 'compile-command) " CSTD=gnu99 LDLIBS=CStandard.c " fn " && ./" fn))
 * End:
 */
