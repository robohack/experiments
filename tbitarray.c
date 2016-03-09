#include <sys/cdefs.h>

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ---- start of bitarray.h ---- */

/*
 * bitarrays -- a simplistic static-sized bit array library
 */

/*
 * You can, in theory, use any integer type as the base type for the array, but
 * it may make sense to choose one which your compiler knows how to most
 * efficiently apply bit-wise operators to (e.g. with any available special CPU
 * bit-manipulating instructions)
 *
 * This is not strictly portable though since only bitfields and unsigned char
 * are necessarily required to have a range of values that entirely represents
 * all of the bits in their object representation.  I.e. The possible range of
 * values that an integer type can represent shall not be calculated from the
 * number of bits in its object representation.
 */
typedef unsigned long bitarr_t;

/* ---- end of bitarray.h ---- */


/* ---- start of bitarray.c ---- */

#define BYTES_PER_SLOT		sizeof(bitarr_t)

#define SLOT_FOR(bit)		((bit) / BYTES_PER_SLOT)/* XXX will this be fast, or should it use a shift? */
#define BIT_NUM_FOR(bit)	((bit) % BYTES_PER_SLOT)
#define BIT_MASK_FOR(bit)	(1U << BIT_NUM_FOR(bit))

void set_bit(bitarr_t [], unsigned int);
int get_bit(bitarr_t [], unsigned int);
int clear_bit(bitarr_t [], unsigned int);

/*
 * Note there is no bounds checking in these functions.  The unsigned index
 * parameter will of course prevent attempts to access negative bit locations,
 * but no attempt is made to prevent access beyond the end of the bit array.
 */

void
set_bit(bitarr_t array[],
	unsigned int bit)
{
	array[SLOT_FOR(bit)] |= BIT_MASK_FOR(bit);
}

int
get_bit(bitarr_t array[],
	unsigned int bit)
{
	return((array[SLOT_FOR(bit)] & BIT_MASK_FOR(bit)) != 0);
}

int
clear_bit(bitarr_t array[],
	  unsigned int bit)
{
	int	obit = get_bit(array, bit);

	array[SLOT_FOR(bit)] &= ~BIT_MASK_FOR(bit);

	return(obit);
}
/* ---- end of bitarray.c ---- */

/* start of example usage */

/*
 * the number of bits in the array -- must be defined at compile time!
 */
#define NUM_BITS	1024

/*
 * We cannot handle more than UINT_MAX bits as that's the size of the index
 * parameter we allow in the function parameter definitions above.
 */
#if NUM_BITS > UINT_MAX			/* IFF the preprocessor can even do this! */
# error "Too many bits for your bit array type!"
#endif

/*
 * example definition for storage for a bit array using bitarr_t:
 *
 * (may result in allocating 1 extra slot, which will be ignored)
 */
bitarr_t my_bits[(NUM_BITS / BYTES_PER_SLOT) + 1] = { 0 };


int main(void);

int
main(void)
{
#ifdef FAST_CLEAR
	memset(my_bits, '0', sizeof(my_bits));
#else
	unsigned int i;

	for (i = 0; i < NUM_BITS; i++) {
		(void) clear_bit(my_bits, i);
	}
#endif

	printf("my_bits[221B] = %d\n", get_bit(my_bits, 221));

	set_bit(my_bits, 221);

	printf("my_bits[221B] = %d\n", get_bit(my_bits, 221));
	printf("my_bits[221B] = %d\n", clear_bit(my_bits, 221));
	printf("my_bits[221B] = %d\n", get_bit(my_bits, 221));

	exit(0);
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
