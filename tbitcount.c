/*
 * cheap tricks to try to to count the number of bits in various C types.
 * (without using CHAR_BIT, but with using it to test  :-)
 *
 * These seem to work as long as you know how wide the largest integer
 * type supported by your compiler is and you can define the needed
 * constants as that width....
 */

#include <assert.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define HAVE_LONG_LONG	/* defined */

#if !defined(HAVE_LONG_LONG)
# define _FOFO_MASK	0x0f0f0f0fL		/* alternating nibbles */
# define _SEVEN_MASK	0x77777777UL		/* alternating nibbles with high bit off */
# define _THREE_MASK	0x33333333UL		/* alternating pairs of bits */
# define _ONE_MASK	0x11111111UL		/* nibbles with low bit only */
#else
# define _FOFO_MASK	0x0f0f0f0f0f0f0f0fLL
# define _SEVEN_MASK	0x7777777777777777ULL
# define _THREE_MASK	0x3333333333333333ULL
# define _ONE_MASK	0x1111111111111111ULL
#endif

/*
 * these are silly
 */
#define CHAR_BITCOUNT		_CHAR_BC((char) -1)
#define SHORT_BITCOUNT		_SHORT_BC((short) -1)
#define INT_BITCOUNT		_INT_BC((int) -1)
#define LONG_BITCOUNT		_LONG_BC((long) -1)	/* fails on sparc64(?) */
#if defined(HAVE_LONG_LONG)
# define LONG_LONG_BITCOUNT	_LONG_LONG_BC((long long) -1)
#endif

#ifndef UINTMAX_C
# if defined(HAVE_LONG_LONG)
#  define UINTMAX_C(c)		c ## ULL
# else
#  define UINTMAX_C(c)		c ## UL
# endif
#endif

#define _BX(type, x)		((unsigned type) (x) - (((unsigned type) (x) >> 1) & (unsigned type) _SEVEN_MASK) \
						     - (((unsigned type) (x) >> 2) & (unsigned type) _THREE_MASK) \
				                     - (((unsigned type) (x) >> 3) & (unsigned type) _ONE_MASK))

/*
 * these don't really work right for anything but the type they are named after
 *
 * (and they're not useful for #if expressions either)
 */
#define _CHAR_BC(x)		(((_BX(char, x) + (_BX(char, x) >> 4)) & (char) _FOFO_MASK) % 255)
#define _SHORT_BC(x)		(((_BX(short, x) + (_BX(short, x) >> 4)) & (short) _FOFO_MASK) % 255)
#define _INT_BC(x)		(((_BX(int, x) + (_BX(int, x) >> 4)) & (int) _FOFO_MASK) % 255)
#define _LONG_BC(x)		(((_BX(long, x) + (_BX(long, x) >> 4)) & (long) _FOFO_MASK) % 255)
#if defined(HAVE_LONG_LONG)
# define _LONG_LONG_BC(x)	(((_BX(long long, x) + (_BX(long long, x) >> 4)) & (long long) _FOFO_MASK) % 255)
#endif

/* http://aggregate.org/MAGIC/ */
unsigned int
ones32(register unsigned int x)
{
	/*
	 * 32-bit recursive reduction using SWAR... but first step is mapping
	 * 2-bit values into sum of 2 1-bit values in sneaky way
	 */
	x -= ((x >> 1) & 0x55555555);
	x = (((x >> 2) & 0x33333333) + (x & 0x33333333));
	x = (((x >> 4) + x) & 0x0f0f0f0f);
	x += (x >> 8);
	x += (x >> 16);

	return(x & 0x0000003f);
}

int main(int, char *[]);

/* ARGSUSED */
int
main(int argc		__attribute__((unused)),
     char *argv[]	__attribute__((unused)))
{
	unsigned int bits;
	int char_bits;
	uint8_t reg08;
	uint16_t reg16;
	uint32_t reg32;
	uint64_t reg64;

	char_bits = -1;
	bits = 1;
	while ((~0U & (~0U >> (bits - 1))) != 1) {
		bits++;
	}
	if (bits % sizeof(unsigned int) == 0) {
		char_bits = bits / sizeof(unsigned int);
	}
	printf("char bits = %u, really %d %s [CHAR_BIT == %u].\n", CHAR_BITCOUNT, char_bits,
	       char_bits == CHAR_BIT ? "==" : "!=", CHAR_BIT);

	bits = 1;
	for (reg08 = ~0; reg08 *= 2; bits++) {
		continue;
	}
	printf("reg08 bits = %d\n", bits);
	bits = 1;
	reg08 = ~0;
	while ((reg08 & (reg08 >> (bits - 1))) != 1) {
		bits++;
	}
	printf("reg08 bits = %d\n", bits);

	bits = 1;
	for (reg16 = ~0; reg16 *= 2; bits++) {
		continue;
	}
	printf("reg16 bits = %d\n", bits);
	bits = 1;
	reg16 = ~0;
	while ((reg16 & (reg16 >> (bits - 1))) != 1) {
		bits++;
	}
	printf("reg16 bits = %d\n", bits);

	bits = 1;
	for (reg32 = ~0; reg32 *= 2; bits++) {
		continue;
	}
	printf("reg32 bits = %d\n", bits);
	bits = 1;
	reg32 = ~0;
	while ((reg32 & (reg32 >> (bits - 1))) != 1) {
		bits++;
	}
	printf("reg32 bits = %d\n", bits);

	bits = 1;
	for (reg64 = ~0; reg64 *= 2; bits++) {
		continue;
	}
	printf("reg64 bits = %d\n", bits);
	bits = 1;
	reg64 = ~0;
	while ((reg64 & (reg64 >> (bits - 1))) != 1) {
		bits++;
	}
	printf("reg64 bits = %d\n", bits);

	printf("short bits = %d, really %d.\n", (int) SHORT_BITCOUNT, (int) sizeof(short) * CHAR_BIT);
	printf("int bits = %d, really %d.\n", (int) INT_BITCOUNT, (int) sizeof(int) * CHAR_BIT);
	printf("long bits = %d, really %d.\n", (int) LONG_BITCOUNT, (int) sizeof(long) * CHAR_BIT);
#if defined(HAVE_LONG_LONG)
	printf("long long bits = %d, really %d.\n", (int) LONG_LONG_BITCOUNT, (int) sizeof(long long) * CHAR_BIT);
#endif

	printf("u char max: %u.\n", (unsigned int) ((unsigned char) UINTMAX_C(~0)));
	printf("u short max: %u.\n", (unsigned int) ((unsigned short) UINTMAX_C(~0)));
	printf("u int max: %llu.\n", (unsigned long long) ((unsigned int) UINTMAX_C(~0)));
	printf("u long max: %llu.\n", (unsigned long long) ((unsigned long) UINTMAX_C(~0)));
#if defined(HAVE_LONG_LONG)
	printf("u long long max: %llu.\n", (unsigned long long) ((unsigned long long) UINTMAX_C(~0)));
#endif

#if 0
	/*
	 * bogus
	 */
	printf("time_t (long?) bits = %d, really %d.\n", (int) _LONG_BC((time_t) -1), (int) sizeof(time_t) * CHAR_BIT);
	printf("time_t (long long?) bits = %d, really %d.\n", (int) _LONG_LONG_BC((time_t) -1), (int) sizeof(time_t) * CHAR_BIT);
#endif
	exit(0);
	/* NOTREACHED */
}

/*

on NetBSD/Alpha:

char bits = 8, really 8.
short bits = 16, really 16.
int bits = 32, really 32.
long bits = 64, really 64.
long long bits = 64, really 64.
uchar max: 255.
ushort max: 65535.
uint max: 4294967295.
ulong max: 18446744073709551615.
ulong long max: 18446744073709551615.


on NetBSD/i386 (gcc 2.95.3):

char bits = 8, really 8.
short bits = 16, really 16.
int bits = 32, really 32.
long bits = 32, really 32.
long long bits = 64, really 64.
uchar max: 255.
ushort max: 65535.
uint max: 4294967295.
ulong max: 4294967295.
ulong long max: 18446744073709551615.


on NetBSD/amd64 (gcc 4.1.3):

char bits = 8, really 8.
short bits = 16, really 16.
int bits = 32, really 32.
long bits = 64, really 64.
long long bits = 64, really 64.
u char max: 255.
u short max: 65535.
u int max: 4294967295.
u long max: 18446744073709551615.
u long long max: 18446744073709551615.


on NetBSD/sparc (gcc 2.95.3):

char bits = 8, really 8.
short bits = 16, really 16.
int bits = 32, really 32.
long bits = 32, really 32.
long long bits = 64, really 64.
uchar max: 255.
ushort max: 65535.
uint max: 4294967295.
ulong max: 4294967295.
ulong long max: 18446744073709551615.

on IRIX 6.5 IP32 (MIPSpro Compilers: Version 7.3.1.2m):

char bits = 8, really 8.
short bits = 16, really 16.
int bits = 32, really 32.
long bits = 32, really 32.
long long bits = 64, really 64.
uchar max: 255.
ushort max: 65535.
uint max: 4294967295.
ulong max: 4294967295.
ulong long max: 18446744073709551615.

 */

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm " fn "; " (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
