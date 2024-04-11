#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/*
 *	If an int can represent all values of the original type, the value is
 *	converted to an int; otherwise, it is converted to an unsigned
 *	int.  These are called the integer promotions. All other types are
 *	unchanged by the integer promotions.
 *
 * Thus following code will return 65536 on 32-bit platforms, but 0 on 16-bit
 * platforms.
 *
 * (Note:  The possible range of values that an integer type can represent
 * can not necessarily be calculated from the number of bits in its object
 * representation.)
 */
static unsigned int
sum(void)
{
	uint16_t a = 65535;
	uint16_t b = 1;

	return (unsigned int) (a + b);
}

static uint32_t
sum32(void)
{
	uint16_t a = 65535;
	uint16_t b = 1;

	return (uint32_t) (a + b);
}

/*
 * this always returns 0 as this is what's really happening on the 16-bit
 * platform since the promotion of 'a' and 'b' leaves them as a 16-bit uint,
 * and the sum overflows leaving all 16-bits as zeros, as is simulated by the
 * cast shown here.
 */
static uint16_t
sum16(void)
{
	uint16_t a = 65535;
	uint16_t b = 1;

	return (uint16_t) (a + b);
}

int
main(void);

int
main()
{
	printf("sum = %" PRIu32 "\n", sum());
	printf("sum = %u\n", (unsigned int) sum());
	printf("sum32 = %" PRIu32 "\n", sum32());
	printf("sum32 = %u\n", (unsigned int) sum32());
	printf("sum16 = %" PRIu16 "\n", sum16());
	printf("sum16 = %u\n", (unsigned int) sum16());

	/* xxx absolute value of INT_MIN is INT_MIN */

	exit(0);
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm -f " fn "; " (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
