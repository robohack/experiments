#include <stdio.h>
#include <stdlib.h>

#define BOO(foo, bar) printf(#foo " = " #bar "\n")

int main(void);

int
main()
{
	BOO(num, nuts);

	exit(0);
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
