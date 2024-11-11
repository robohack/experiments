#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>

int main(void);

int
main()
{
	char *loc;

	loc = setlocale(LC_ALL, "");

	printf("setlocale() = '%s': %s\n", loc, XSupportsLocale() ? "OK" : "Nope!");

	exit(0);
}


/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm -f " fn "; " (default-value 'compile-command) " CPPFLAGS=-I/usr/X11R7/include LDLIBS='-L/usr/X11R7/lib -lX11 -lxcb -lXau -lXdmcp' " fn " && ./" fn))
 * End:
 */
