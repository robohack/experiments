#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(void);

static void
from_array_to_string(void)
{
	struct foo {
		char name[8];	/* an array with an up-to-8-char name */
	} src = { { '1', '2', '3', '4', '5', /* '6', '7', '8' */ } };

	char *dst;

	dst = malloc(sizeof(src.name) + 1);

	/* from a char array to a string */
	strncpy(dst, src.name, sizeof(src.name));
	dst[8] = '\0';

	printf("dst = '%s'\n", dst);
}

static void
from_string_to_array(void)
{
	const char *src = "1234567890";

	struct foo {
		char name[8];	/* an array with an up-to-8-char name */
	} dst;

	/* from a string to a char array */
#pragma GCC diagnostic push
	/* xxx avoid unhelpful: warning: ‘__builtin_strncpy’ output truncated
	 * copying 8 bytes from a string of length 10 [-Wstringop-truncation] */
#pragma GCC diagnostic ignored "-Wstringop-truncation"
	strncpy(dst.name, src, sizeof(dst.name));
#pragma GCC diagnostic pop

	printf("dst = '%.*s'\n", (int) sizeof(dst.name), dst.name);
}

static void
from_untrusted_string(void)
{
	char *src;
	char dst[8 + 1];

	src = malloc(BUFSIZ);
	strcpy(src, "1234567890");

	strncpy(dst, src, 8);
	dst[8] = '\0';

	printf("dst = '%s'\n", dst);
}

int
main()
{
	from_array_to_string();
	from_string_to_array();
	from_untrusted_string();

	exit(0);
}


/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm -f " fn "; " (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
