#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <vis.h>

char *binary_fmt(unsigned int, int);

char *
binary_fmt(unsigned int x,
	   int zf)			/* natural promoted type of bool  */
{
	static char str[(sizeof(x) * CHAR_BIT) + 1];
	char *s = str + sizeof(str) - 1;

	*str = *s = '\0';
	do {
#if 0	/* oh wonderful compiler, which is best? */
		*--s = '0' + x % 2;
#else
		*--s = '0' + (x & 1);
#endif
#if 0	/* oh wonderful compiler, which is best? */
		x /= 2;
#else
		x >>= 1;
#endif
	} while (x);

	if (zf) {
		while (s > str)
			*--s = '0';
	}

	return s;
}

/*
 * CRC-16 CCITT
 */

#define INIT_CRC_WITH_AUGMENTED_ZEROS	/* defined */

static const unsigned short crc16_ccitt_false_table[256]= {
	0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
	0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
	0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
	0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
	0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
	0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
	0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
	0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
	0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
	0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
	0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
	0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
	0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
	0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
	0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
	0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
	0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
	0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
	0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
	0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
	0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
	0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
	0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
	0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
	0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
	0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
	0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
	0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
	0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
	0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
	0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
	0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
};

static uint16_t
crc16_ccitt_false(const unsigned char *p,
                  size_t len)
{
	uint16_t crc;

#ifdef INIT_CRC_WITH_AUGMENTED_ZEROS
	crc = 0x1D0F;
#else
	crc = ~0U;
#endif

	while (len--) {
		crc = (crc << 8) ^ crc16_ccitt_false_table[(crc >> 8) ^ *p++];
	}

	return crc;
}

/*
 * CRC-16 CCITT raw calculation for X.25
 *
 * CCITT CRC-16 polynomial is:  x^16 + x^12 + x^5 + 1
 *
 * This is 0x1021, and when masked and reversed this is 0x8408, and so can be
 * used with the algorithm below to generate the compliment of the CRC
 *
 * (which is also apparently:  x^16 + x^15 + x^10 + x^3)
 */
static uint16_t
update_crc16_ccitt_raw_x25(uint32_t crc,
                           const unsigned char *buffer,
                           size_t len)
{
	if (len == 0) {
		return crc;
	}
	do {
		uint_fast8_t i;
		unsigned char data;

		/* for each bit in data, LSB to MSB.... */
		for (i = 0, data = *buffer++;
		     i < 8;
		     i++, data >>= 1) {
			/* if LSB xor LSB */
			if ((crc & 1) ^ (data & 1)) {
				/* crc = CRC/2 xor REVPOLY */
				crc = (crc >> 1) ^ (uint16_t) 0x8408;
			} else {
				/* crc = CRC/2 */
				crc >>= 1;
			}
		}
	} while (--len > 0);

	/* ones compliment */
	crc = ~crc;

	return crc;
}

static void
check(const unsigned char *buf,
      size_t len,
      unsigned int expected)
{
	uint16_t crc;
	char *bvis;

	bvis = malloc(len * 4);
	strvisx(bvis, (const char *) buf, len, VIS_SAFE);

	crc = (uint16_t) ~0U;		/* initial value */
	crc = update_crc16_ccitt_raw_x25(crc, buf, len);
	printf("'%s'[%lu]: crc_x25   = 0x%04x, %s 0x%04x(expected)\n",
	       bvis, (unsigned long ) len,
	       (unsigned int) crc,
	       crc == expected ? "==" : "!=",
	       expected);

	crc = crc16_ccitt_false(buf, len);
	printf("'%s'[%lu]: crc_false = 0x%04x, %s 0x%04x(expected)\n\n",
	       bvis, (unsigned long) len,
	       (unsigned int) crc,
	       crc == expected ? "==" : "!=",
	       expected);

	free(bvis);

	return;
}

int
main(void);

int
main()
{
	unsigned int i;
	unsigned char buf[BUFSIZ];

	printf("~0x1021 = %s\n", binary_fmt(0x11021, true));
	printf("~0x0811 = %s\n", binary_fmt(0x0811, true));

	memset(buf, '\0', sizeof(buf));
	check(buf, 0, 0xFFFF);
	check(buf, 2, 0x1D0F);

	memset(buf, '\0', sizeof(buf));
	buf[0] = 'A';
	check(buf, 1, 0xB915);

	memset(buf, '\0', sizeof(buf));
	buf[2] = 'A';
	check(buf, 3, 0x9479);

	memset(buf, '\0', sizeof(buf));
	buf[0] = 0x03;			/* CCITT X.25 1984 Appendix 1 */
	buf[1] = 0x3F;
	check(buf, 2, 0x5BEC);

	memset(buf, '\0', sizeof(buf));
	buf[2] = 0x03;
	buf[3] = 0x3F;
	check(buf, 4, 0x1FC2);

	memset(buf, '\0', sizeof(buf));
	buf[0] = 'T';
	check(buf, 1, 0xFB81);

	memset(buf, '\0', sizeof(buf));
	buf[2] = 'T';
	check(buf, 3, 0xFB81);

	memset(buf, '\0', sizeof(buf));
	snprintf((char *) buf, sizeof(buf), "%s", "123456789");
	check(buf, strlen((const char *) buf),  0x29B1);

	memset(buf, '\0', sizeof(buf));
	snprintf((char *) buf + 2, sizeof(buf) - 2, "%s", "123456789");
	check(buf, strlen((const char *) buf + 2) + 2,  0xE5CC);

	memset(buf, '\0', sizeof(buf));
	snprintf((char *) buf, sizeof(buf), "%s", "TEST");
	check(buf, strlen((const char *) buf),  0x8CCA);

	memset(buf, '\0', sizeof(buf));
	snprintf((char *) buf + 2, sizeof(buf) - 2, "%s", "TEST");
	check(buf, strlen((const char *) buf + 2) + 2,  0x061A);

	memset(buf, '\0', sizeof(buf));
	for (i = 0; i < 256; i++) {
		buf[i] = 'A';
	}
	printf("strlen(A's) = %d\n", strlen((const char *) buf));
	check(buf, strlen((const char *) buf), 0xE938); /* xxx ??? */

	memset(buf, '\0', sizeof(buf));
	for (i = 2; i < 258; i++) {
		buf[i] = 'A';
	}
	printf("strlen(A's) = %d\n", strlen((const char *) buf + 2) + 2);
	check(buf, strlen((const char *) buf + 2) + 2, 0xE938); /* xxx ??? */

	exit(EXIT_SUCCESS);
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm -f " fn "; " (default-value 'compile-command) " " fn " && ./" fn " < /dev/null"))
 * End:
 */
