#include <limits.h>			/* needed for CHAR_BIT */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static char *
hexencode(time_t tm, uint64_t id)
{
	const char *hexchars = "0123456789ABCDEF";
	uint8_t bin[2 * sizeof(uint64_t)];
	uint64_t ti;
	static char buf[sizeof(bin) * 2 + 1];
	unsigned int z;
	unsigned int s;

	/* some ILP32 systems _may_ have 32-bit time_t */
	ti = (uint64_t) tm;

	/* fill bin[] with alternating bytes, ti reversed */
#if 1
	for(z = 0, s = sizeof(uint64_t) - 1; z < sizeof(uint64_t); z++, s--) {
		bin[z*2]     = ti >> (z * CHAR_BIT) & 0xff;
		bin[z*2 + 1] = id >> (s * CHAR_BIT) & 0xff;
		printf("bin[%u] = %02x, ti >> %u\n", z * 2,     ti >> (z * 8) & 0xff, z * 8);
		printf("bin[%u] = %02x, id >> %u\n", z * 2 + 1, id >> (s * 8) & 0xff, s * 8);
	}
#else
	bin[0] = ti >> 0 & 0xff;		/* Tm 8 */
	bin[1] = id >> 56 & 0xff;		/* Id 1 */
	bin[2] = ti >> 8 & 0xff;		/* Tm 7 */
	bin[3] = id >> 48 & 0xff;		/* Id 2 */
	bin[4] = ti >> 16 & 0xff;		/* Tm 6 */
	bin[5] = id >> 40 & 0xff;		/* Id 3 */
	bin[6] = ti >> 24 & 0xff;		/* Tm 5 */
	bin[7] = id >> 32 & 0xff;		/* Id 4 */
	bin[8] = ti >> 32 & 0xff;		/* Tm 4 */
	bin[9] = id >> 24 & 0xff;		/* Id 5 */
	bin[10] = ti >> 40 & 0xff;		/* Tm 3 */
	bin[11] = id >> 16 & 0xff;		/* Id 6 */
	bin[12] = ti >> 48 & 0xff;		/* Tm 2 */
	bin[13] = id >> 8 & 0xff;		/* Id 7 */
	bin[14] = ti >> 56 & 0xff;		/* Tm 1 */
	bin[15] = id >> 0 & 0xff;		/* Id 8 */
#endif
	/* encode bin[] as hexadecimal into buf[] */
	for (z = 0; z < sizeof(bin); z++) {
		buf[z*2]     = hexchars[(bin[z] & 0xF0) >> 4];
		buf[z*2 + 1] = hexchars[ bin[z] & 0x0F ];
	}
	return buf;
}

static uint8_t nibble2byte(char n)
{
	if(n >= '0' && n <= '9') return n - '0';
	if(n >= 'a' && n <= 'f') return n - 'a' + 10;
	if(n >= 'A' && n <= 'F') return n - 'A' + 10;

	return 255;
}

static uint8_t hex2byte(char n1, char n0)
{
	uint8_t b = 0;

	b |= (nibble2byte(n1) << 4) & 0xF0;
	b |= (nibble2byte(n0) << 0) & 0x0F;

	return b;
}

/*
 * Extract the alternating ID bytes and re-form the ID integer value.
 *
 * e.g. bytes:  ff 00 ee 11 dd 22 cc 33 bb 44 aa 55 99 66 88 77
 * byte index:  01 23 45 67 89 .. .. .. .. .. .. .. .. .. .. ..
 *                             1011 1213 1415 1617 1819 2021 2223 2425 2627 2829 3031
 * hex pair:     0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15
 * assignment:  t8 i1 t7 i2 t6 i3 t5 i4 t4 i5 t3 i6 t2 i7 t1 i8
 * shifts:      00 56 08 48 16 40 24 32 32 24 40 16 48 08 56 00
 */
static uint64_t
hexdecode(char *buf)
{
	int z;
	unsigned int s;
	uint64_t i;

	for (i = 0, z = 31, s = 0; z >= 3; z -= 4, s += 8) {
		i |= (uint64_t) hex2byte(buf[z - 1], buf[z]) << s;
		printf("buf[%u], %x << %u, %jx\n", z, (unsigned int) hex2byte(buf[z - 1], buf[z]), s, (uintmax_t) i);
	}

	return i;
}


int
main(void)
{
	time_t t;
	uint64_t id;
	const char *buf;
	uint64_t idd;

	t = time(NULL);
	id = 0x00000013E5B169A4;

	buf = hexencode(t, id);
	puts(buf);

	idd = hexdecode(buf);
	printf("%jx\n", (uintmax_t) idd);

	exit(id != idd);
}
