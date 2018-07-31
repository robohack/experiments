#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static uint8_t
checksum8mod255(const unsigned char *p,
                size_t len)
{
	uint8_t csum = 0;

	while (len--) {
		csum = (csum + *p++) % 255;
	}

	return csum;
}


int
main(void);

int
main()
{
	const char *test = " 1=0= 150= 1307=   0=";

	printf("checksum8mod255() = %u\n", checksum8mod255(test, strlen(test));

	exit(EXIT_SUCCESS);
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm " fn "; " (default-value 'compile-command) " " fn " && ./" fn " < /dev/null"))
 * End:
 */
