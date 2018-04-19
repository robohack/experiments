#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <pwd.h>
#include <unistd.h>

/*
 * some hacks to parse ASCII-hexadecimal-encoded values
 *
 * Note the 0x0fU mask in ASCII_HEX_TO_4() which effectively makes sure we only
 * use the number of bits in the input value that will be used in the shift by
 * the next level macro, and for C89 and newer it must be explictly unsigned.
 *
 * ASCII_HEX_TO_4() can make use of input values which are outside the normal
 * hexadecimal range, but the next level macros could not make sensible use of
 * these values as they do not, and can not, know how to intuit how far to shift
 * the result given by ASCII_HEX_TO_4()
 */
#define UL(b)			(b >= 'a' ? 'a' : 'A')
#define ASCII_HEX_TO_4_(b)	(0x0fU & (unsigned char) ((b >= '0' && b <= '9') ? (b - '0') : (b - UL(b)) + 10))
#define ASCII_HEX_TO_4(b)	((uint8_t) ASCII_HEX_TO_4_(b))
#define ASCII_HEX_TO_8_(b1, b2)	(((uint8_t) ASCII_HEX_TO_4(b1) << 4) | ASCII_HEX_TO_4(b2))
#define ASCII_HEX_TO_8(b1, b2)	((uint8_t) ASCII_HEX_TO_8_(b1, b2))
#define ASCII_HEX_TO_16_(b1, b2, b3, b4)	(((uint16_t) ASCII_HEX_TO_8_(b1, b2) << 8) | ASCII_HEX_TO_8_(b3, b4))
#define ASCII_HEX_TO_16(b1, b2, b3, b4)		((uint16_t) ASCII_HEX_TO_16_(b1, b2, b3, b4))
#define ASCII_HEX_TO_32_(b1, b2, b3, b4,								\
                         b5, b6, b7, b8)	((((uint32_t) ASCII_HEX_TO_16_(b1, b2, b3, b4) << 16) | \
	                                           (uint32_t) ASCII_HEX_TO_16_(b5, b6, b7, b8)))
#define ASCII_HEX_TO_32(b1, b2, b3, b4, b5, b6, b7, b8)	(uint32_t) ASCII_HEX_TO_32_(b1, b2, b3, b4, b5, b6, b7, b8)


int main(void);

/* ARGSUSED */
int
main()
{
	const char *a;
	uint8_t u8;
	uint16_t u16;
	uint32_t u32;

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

	a = "000000FF";
	u32 = ASCII_HEX_TO_32(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7]);
	printf("%s = %lu\n", a, (unsigned long) u32);

	a = "2b3c4d5e";
	u32 = ASCII_HEX_TO_32(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7]);
	printf("%s = %lu\n", a, (unsigned long) u32);

	a = "ffffffff";
	u32 = ASCII_HEX_TO_32(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7]);
	printf("%s = %lu\n", a, (unsigned long) u32);

	a = "UINT32_MAX";
	u32 = UINT32_MAX;
	printf("FYI, %s = %lu\n", a, (unsigned long) u32);

	exit(0);
	/* NOTREACHED */
}


/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm " fn "; " (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
