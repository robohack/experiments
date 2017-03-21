/*
 * From Rob Pike:
 *
 * If the data stream encodes values with byte order B, then the algorithm to
 * decode the value on computer with byte order C should be about B, not about
 * the relationship between B and C.
 *
 * Let's say your data stream has a little-endian-encoded 32-bit [[thus
 * unsigned]] integer.  Here's how to extract it (assuming unsigned bytes):
 *
 *	ui = (data[0]<<0) | (data[1]<<8) | (data[2]<<16) | (data[3]<<24);
 *
 * If it's [[the data stream is]] big-endian, here's how to extract it:
 *
 *	ui = (data[3]<<0) | (data[2]<<8) | (data[1]<<16) | (data[0]<<24);
 *
 * Both these snippets work on any machine, independent of the machine's byte
 * order, independent of alignment issues, independent of just about anything.
 * They are totally portable, given unsigned bytes and 32-bit integers.
 *
 * [[ .... ]]
 *
 * The entire Plan 9 system ran, without architecture-dependent #ifdefs of any
 * kind, on dozens of computers of different makes, models, and byte orders.  I
 * promise you, your computer's byte order doesn't matter even at the level of
 * the operating system.
 *
 * <URL:http://commandcenter.blogspot.com.au/2012/04/byte-order-fallacy.html>
 */

/*
 * see also tgetlong.c for an example of reading a big-endian long from a stdin
 * stream
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/* macros to extract little-endian integers from a byte buffer */
#define PULL2(p)	((p)[0] + ((uint16_t) ((p)[1]) << 8))
#define PULL4(p)	(PULL2(p) + ((uint32_t) PULL2(p + 2) << 16))
#define PULL8(p)	(PULL4(p) + ((uint64_t) PULL4(p + 4) << 32))

/* macros to store integers into a byte buffer in little-endian order */
#define PUT2(p, a)	do {(p)[0] = a; (p)[1] = (a) >> 8;} while(0)
#define PUT4(p, a)	do {PUT2(p, a); PUT2(p + 2, a >> 16);} while(0)
#define PUT8(p, a)	do {PUT4(p, a); PUT4(p + 4, a >> 32);} while(0)

int main(void);

int
main(void)
{
	unsigned long x;
	unsigned char *p;
	unsigned int i;

	if (sizeof(long) <= 4) {
		x = 0x01020304UL;	/* xxx never executed on I64 machines */
	} else {
		x = (unsigned long) 0x0102030405060708ULL;	/* unsigned long long (ULL) to make the constant portable */
	}
	p = (unsigned char *) &x;

	putchar('\n');
	for (i = 0; i < sizeof(x); i++) {
		printf("%x ", *p++);
	}
	putchar('\n');

	exit(0);
}

/*
 * output examples:
 *
 *	1 2 3 4 => big endian
 *	4 3 2 1 => little endian
 *	2 1 4 3 => PDP-11
 */


/*
 * some thoughts regarding bytes and bits and memory-mapped hardware registers:
 * 
 * There's essentially no difference between bitfields and bit-wise operations
 * with respect to safely operating directly on memory-mapped hardware
 * registers.  With either data type the compiler may still do the actual
 * storage references in unpredictable ways, even on hardware that might have
 * instructions directly suited to such manipulations.  Safe code must do
 * atomic reads and writes, perhaps even using assembler interface functions,
 * before/after manipulating the bits or packed-int values using C-level
 * expressions.  Even if your compiler appears to do the right thing when you
 * examine sample generated code slight perturbations in your code might
 * drastically change the generated code, particularly if you have optimization
 * enabled at any level.
 *
 * The only way that either bit-wise or bitfield operations will ever be safe
 * when done from C directly on memory-mapped hardware registers is when your
 * compiler offers a non-standard extension which turns on promises about such
 * operations.
 *
 * "volatile" is _not_ such a feature.
 *
 * In theory a variable declared "volatile" will not have any manipulations
 * extending beyond the well defined sequence points in the code structure,
 * _but_ there may be optimizations or changes between sequence points.
 *
 * I.e. quoting from Harbison and Steele (emphasis mine):  "References and
 * modifications of volatile objects must not be optimized _across_ sequence
 * points, although optimizations between sequence points are permitted.  Extra
 * references and modifications beyond those appearing in the source code _are_
 * allowed by the C language standard."
 *
 * So, the only safe way to ensure that references to access or modify a
 * volatile object are "as-written" is to copy the value in/out of a volatile
 * object to separate storage, and to only ever manipulate the copy.
 */

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
