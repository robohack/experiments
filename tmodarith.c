#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int main(void);

/*
 * From Harbison & Steele (5th):
 *
 *	"If the result type is an unsigned type, then the result must be that
 *	unique value of the result type that is equal (congruent) mod 2^n to the
 *	original value, where 'n' is equal to the number of bits used in the
 *	representation of the result type."
 *
 * This means that in Standard C, with value-preserving semantics, there can be
 * differences in the signedness of the result of an expression involving an
 * integral type with the same width as an 'int'.  Perhaps this sucks.  Sign is
 * more important than value, especially since these are edge cases anyway.
 */

/*
 * xxx turn optimization off, or put these in another compilation unit, to avoid
 * constant evaluation at compile time
 */
unsigned int UI;
long int LI;

int
main()
{
#if 1
	uint8_t x;
#else
	uint16_t x = -3;
#endif
	int y1, y2;
	unsigned int z1, z2;

	x = (UINT8_MAX + 1) - 3;
	printf("initial:  x = %u\n", x);

	x *= x;
	/* a Standard ISO C compiler should evaluate the statement above as
	 * equivalent to:
	 */
#if 0
	x = (unsigned short) ((int) x * (int) x);
#endif
	/* a traditional signedness-preserving C compiler would evaluate the
	 * statement above as equivalent to:
	 */
#if 0
	x = (unsigned short) ((unsigned int) x * (unsigned int) x);
#endif
	printf("x * x = %u\n", x);

	x = (UINT8_MAX + 1) - 3;
	y1 = x;
	y2 = x;
	x = y1 * y2;

	printf("y1 * y2 = %u\n", x);

	x = (UINT8_MAX + 1) - 3;
	z1 = x;
	z2 = x;
	x = z1 * z2;

	printf("z1 * z2 = %u\n", x);

	/*
	 * Standard C so-called "value preserving"
	 *
	 * See H&S 4th edition, p. 175
	 */ {
		unsigned short S;
		uint32_t I;	/* xxx i.e. short where sizeof(short) == sizeof(int) */

		signed int sr;
		unsigned int ur;

		S = 1;
		I = 1;

		/* if sizeof(short) < sizeof(int):  result type int, value -1 */
		sr = -S;
		ur = -S;		/* xxx should the compiler warn??? */
		printf("sr = -S: %d\n", sr);
		printf("ur = -S: %u\n", ur);

		/* if sizeof(short) == sizeof(int):  result type unsigned, value 2^n */
		sr = -I;		/* xxx should the compiler warn??? */
		ur = -I;
		printf("sr = -I: %d\n", sr);
		printf("ur = -I: %u\n", ur);

		/* from p. 198 H&S 5th edition */
		UI = -1;		/* GCC: "negative integer implicitly converted to unsigned type" */
		LI = 0;

		if (UI < LI) {
			puts("long + unsigned == long");
		} else {
			puts("long + unsigned == unsigned");
		}
	}

	exit(0);
	/* NOTREACHED */
}
