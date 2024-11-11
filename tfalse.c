#define EXIT_VALUE 1
#include "ttrue.c"

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm -f " fn "; cc -g -Og -static -Wl,-Map=" fn ".map -o " fn " " (buffer-file-name) " && ./" fn))
 * End:
 */
