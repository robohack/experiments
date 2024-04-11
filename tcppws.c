#include <stdio.h>

#define lput(str) fprintf(fp, "%s\n", str)

#define M(x) x ## _

#define F2	M(f2)

void F2(void);

F2()
{
	FILE	*fp;

	lput("");
	lput("foobar.");
	lput("nutbar."                        );
	lput("foobar.");
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm -f " fn "; " (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
