#include <sys/types.h>
#include <vis.h>

main()
{
	char foo[100];

	strvis(foo, "\\\"\abcdef", VIS_CSTYLE);
	puts(foo);
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm " fn "; " (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
