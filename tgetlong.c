#include <stdio.h>
#include <unistd.h>

uint32_t get_be_u32(void);
uint32_t get_le_u32(void);

/*
 * If the external data were written with the most significant byte first
 * (big-endian), the following code will read a 4-byte unsigned integer
 * correctly regardless of the architecture of the executing (reading) machine:
 *
 * See tbyteorder.c for further explanation.
 *
 * There has been some claim that one should be careful to treat the input
 * bytes as signed characters, but that should only matter if the destination
 * is signed.
 *
 * Note that it is more portable to use multiplication by powers of 8 instead
 * of a left-shift for signed values (since shifting a signed value can produce
 * undefined behaviour if it is negative).  This should only be necessary for
 * the MSB (i.e. the byte which contains the sign bit), but here is the
 * pedantic version for getting a signed long:
 *
 *	char c[4];
 *	long d;			// sizeof(long) must be >= sizeof(int32_t)
 *	long i;
 *
 *	i = *((signed char *) (&c[0]));		// do/force sign extension
 *	i *= 1 << (CHAR_BIT*3);
 *	d = i;
 *
 *	i = *((unsigned char *) (&c[1]));	// _avoid_ sign extension
 *	i *= 1 << (CHAR_BIT*2);
 *	d |= i;
 *
 *	i = *((unsigned char *) (&c[2]));	// _avoid_ sign extension
 *	i *= 1 << CHAR_BIT;
 *	d |= i;
 *
 *	i = *((unsigned char *) (&c[3]));	// _avoid_ sign extension
 *	d |= i;
 */
uint32_t
get_be_u32()
{
	uint32_t l;

	/*
	 * Note the careful use of multiple statements -- if these were
	 * combined into one statement there would be no guarantee about the
	 * order of evaluation of the getchar() calls.
	 */
	l = (getchar() & 0xFF) << 030;
	l |= (getchar() & 0xFF) << 020;
	l |= (getchar() & 0xFF) << 010;
	l |= (getchar() & 0xFF) << 000;

	return l;
}

/*
 * If the external data were written with the lease significant byte first
 * (little-endian), the following code will read a 4-byte integer correctly
 * regardless of the architecture of the executing machine:
 */
uint32_t
get_le_u32()
{
	uint32_t l;

	l = (getchar() & 0xFF) << 000;
	l |= (getchar() & 0xFF) << 010;
	l |= (getchar() & 0xFF) << 020;
	l |= (getchar() & 0xFF) << 030;

	return l;
}
