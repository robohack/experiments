#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <pwd.h>
#include <unistd.h>

/*
 * some hacks to parse ASCII-hexadecimal-encoded values
 *
 * Note the 0x0f mask in ASCII_HEX_TO_4() which effectively makes sure we only
 * use the number of bits in the input value that will be used in the shift by
 * the next level macro.
 *
 * ASCII_HEX_TO_4() can make use of input values which are outside the normal
 * hexadecimal range, but the next level macros could not make sensible use of
 * these values as they do not, and can not, know how intuit how far to shift
 * the result given by ASCII_HEX_TO_4()
 */
#define UL(b)			(b >= 'a' ? 'a' : 'A')
#define ASCII_HEX_TO_4(b)	(0x0f & ((b >= '0' && b <= '9') ? b - '0' : b - UL(b) + 10))
#define ASCII_HEX_TO_8(b1, b2)	((ASCII_HEX_TO_4(b1) << 4) | ASCII_HEX_TO_4(b2))
#define ASCII_HEX_TO_16(b1, b2, b3, b4)	((ASCII_HEX_TO_8(b1, b2) << 8) | ASCII_HEX_TO_8(b3, b4))

int main(void);

/* ARGSUSED */
int
main()
{
	char *a;
	uint8_t u8;
	uint16_t u16;

	a = "0";
	u8 = ASCII_HEX_TO_4(a[0]);
	printf("%s = %u\n", a, (unsigned int) u8);

	a = "1";
	u8 = ASCII_HEX_TO_4(a[0]);
	printf("%s = %u\n", a, (unsigned int) u8);

	a = "9";
	u8 = ASCII_HEX_TO_4(a[0]);
	printf("%s = %u\n", a, (unsigned int) u8);

	a = "a";
	u8 = ASCII_HEX_TO_4(a[0]);
	printf("%s = %u\n", a, (unsigned int) u8);

	a = "F";
	u8 = ASCII_HEX_TO_4(a[0]);
	printf("%s = %u\n", a, (unsigned int) u8);

	a = "g";
	u8 = ASCII_HEX_TO_4(a[0]);
	printf("%s = %u\n", a, (unsigned int) u8);

	a = "G";
	u8 = ASCII_HEX_TO_4(a[0]);
	printf("%s = %u\n", a, (unsigned int) u8);

	a = "z";
	u8 = ASCII_HEX_TO_4(a[0]);
	printf("%s = %u\n", a, (unsigned int) u8);

	a = "Z";
	u8 = ASCII_HEX_TO_4(a[0]);
	printf("%s = %u\n", a, (unsigned int) u8);

	a = "1a";
	u8 = ASCII_HEX_TO_8(a[0], a[1]);
	printf("%s = %u\n", a, (unsigned int) u8);

	a = "1A";
	u8 = ASCII_HEX_TO_8(a[0], a[1]);
	printf("%s = %u\n", a, (unsigned int) u8);

	a = "FF";
	u8 = ASCII_HEX_TO_8(a[0], a[1]);
	printf("%s = %u\n", a, (unsigned int) u8);

	a = "GG";
	u8 = ASCII_HEX_TO_8(a[0], a[1]);
	printf("%s = %u\n", a, (unsigned int) u8);

	a = "gg";
	u8 = ASCII_HEX_TO_8(a[0], a[1]);
	printf("%s = %u\n", a, (unsigned int) u8);

	a = "zz";
	u8 = ASCII_HEX_TO_8(a[0], a[1]);
	printf("%s = %u\n", a, (unsigned int) u8);

	a = "ffff";
	u16 = ASCII_HEX_TO_16(a[0], a[1], a[2], a[3]);
	printf("%s = %u\n", a, (unsigned int) u16);

	a = "zzzz";
	u16 = ASCII_HEX_TO_16(a[0], a[1], a[2], a[3]);
	printf("%s = %u\n", a, (unsigned int) u16);

	a = "UINT16_MAX";
	u16 = UINT16_MAX;
	printf("FYI, %s = %u\n", a, (unsigned int) u16);

	exit(0);
	/* NOTREACHED */
}


/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
