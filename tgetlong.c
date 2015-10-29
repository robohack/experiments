/*
 * If the external data were written with the most significant byte first
 * (big-endian), the following code will read a 4-byte integer correctly
 * regardless of the architecture of the executing machine:
 *
 * See tbyteorder.c for further explanation.
 *
 * There has been some claim that one should be careful to treat the input
 * bytes as signed characters.
 *
 * Note that it is more portable to use multiplication by powers of 8 instead
 * of a left-shift for signed values (since shifting a signed value can produce
 * undefined behaviour if it is negative).  This should only be necessary for
 * the MSB (i.e. the byte which contains the sign bit), but here is the
 * pedantic version for getting a signed long:
 *
 *	long l;
 *	int i;
 *
 *	i = *((signed char *) (&c[0]));
 *	i *= 1 << (CHAR_BIT*3);
 *	l = i;
 *	i = *((unsigned char *) (&c[1]));
 *	i *= 1 << (CHAR_BIT*2);
 *	l |= i;
 *	i = *((unsigned char *) (&c[2]));
 *	i *= 1 << CHAR_BIT;
 *	l |= i;
 *	i = *((unsigned char *) (&c[3]));
 *	l |= i;
 */
uint32_t
get_be_long(void)
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
get_le_long(void)
{
	uint32_t l;

	l = (getchar() & 0xFF) << 000;
	l |= (getchar() & 0xFF) << 010;
	l |= (getchar() & 0xFF) << 020;
	l |= (getchar() & 0xFF) << 030;

	return l;
}
