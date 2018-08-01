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

/*
 * "Hxx\r\n"
 *
 * "xx=s=tttt=hhhhh=wwww=ccc\r\n"
 *
 *  xx: probe number
 *  s: gauge status (0=OK, 1=Probe error, 2=Probe doesn't exist)
 *  tttt: temperature in 1/10°C (always 150)
 *  hhhhh: volume in L
 *  wwww: water volume in L (always 0)
 *  ccc: checksum from 1 to 255
 */
int
main()
{
	const char *test  = " 1=0= 150= 1307=   0="; /* 235 XXX is 230!!! */
	const char *test2 = " 2=0= 150= 5750=   0="; /* 237 [correct] */
	uint8_t csum;

	csum = checksum8mod255((const unsigned char *) test, strlen(test));
	printf("checksum8mod255(test1) = %u [0x%02x]\n", csum, csum);

	csum = checksum8mod255((const unsigned char *) test2, strlen(test2));
	printf("checksum8mod255(test2) = %u [0x%02x]\n", csum, csum);

	exit(EXIT_SUCCESS);
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm " fn "; " (default-value 'compile-command) " " fn " && ./" fn " < /dev/null"))
 * End:
 */
