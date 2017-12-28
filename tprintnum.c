#include <assert.h>

void
prntnum(unsigned long n,		/* number to be printed */
        unsigned int base,		/* number base */
        char sign,			/* optional sign character */
        char *outbuf)			/* destination buffer */
{
	int i = 12;
	int j = 0;

	assert(base <= 16);

	do {
		outbuf[i] = "0123456789ABCDEF"[num % base];
		i--;
		n = num / base;
	} while (num > 0);

	if (sign != ' ') {
		outbuf[0] = sign;
		++j;
	}

	while (++i < 13) {
		outbuf[j++] = outbuf[i];
	}

	outbuf[j] = '\0';
}
