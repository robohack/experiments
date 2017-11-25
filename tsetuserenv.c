#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void);

static const char * const input =
	"FOO1=bar,FOO2=\\ bar\\,\\ none,FOO3,,FOO4=boo\\ ";

int
main(void)
{
	char *var;
	char *ptr;
	char *str;
	char *freep;

	str = freep = malloc(strlen(input) + 1);
	strcpy(str, input);

	for (ptr = str; (var = stresep(&ptr, ", \t", '\\')) != NULL;) {
		char *value;

		if (!*var) {
			printf("empty field at '%s'\n", ptr);
			continue;
		}

		if ((value = strchr(var, '=')) != NULL) {
			*value++ = '\0';
		} else {
			value = NULL;
		}
		printf("VAR = '%s', VALUE='%s'\n", var, value ? value : "(NULL)");
	}
	free(freep);

	exit(0);
	/* NOTREACHED */
}


/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm " fn "; " (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
