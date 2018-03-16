#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

int main(void);


/*
 *
 * GNU GCC invoked with the -fwrapv command-line option defines the same modulo
 * arithmetic for both unsigned and signed integers.
 *
 * GNU GCC invoked with the -ftrapv command-line option causes a trap to be
 * generated when a signed integer overflows, which will most likely abnormally
 * exit.  On a UNIX system, the result of such an event may a call to abort().
 *
 * GNU GCC invoked without either the -fwrapv or the -ftrapv option _may_ simply
 * assume that signed integers never overflow and may generate object code
 * accordingly.
 *
 */


int
main()
{
	unsigned int ufirst;
	unsigned int usecond;

	int first;
	int second;

	long lsec = 0;

	ufirst = UINT_MAX - 100U;
	usecond = ufirst + 200U;

	printf("ufirst = %u, usecond = %u, first + 200 = %u, diff = %u\n",
	       ufirst, usecond, ufirst + 200, usecond - ufirst);

	first = INT_MAX - 100;
	second = first + 200;

	printf("first = %d, second = %d (%d), first + 200 = %d, diff = %d\n",
	       first, second, second & INT_MAX, first + 200, second - first);

	/* first turn off the sign bit for the wrapped value */
	second = ((unsigned int) second) << 1;
	second = ((unsigned int) second) >> 1;
	/* now add the amount it wrapped by... */
	lsec = second;
	if (first > second) {
		lsec += (long) INT_MAX + 1;
	}

	printf("first = %d, second = %d, first + 200 = %ld\n", first, second, lsec);

	/*
	 * What happens if the statement is of the form:
	 *
	 *	<signed int> = <unsigned int> <op> <signed int>
	 *
	 * and the signed int on the RHS is negative?  Does it matter if the int
	 * on the LHS is signed or unsigned?
	 *
	 * (the RHS signed int is cast to unsigned and the result is cast to signed)
	 *
	 * Most worrisome is the case where a < b and where a - b would be
	 * negative if it weren't for the fact that one of the operands was
	 * unsigned (e.g. size_t) [and even if LHS is signed, but larger].
	 *
	 * Conclusion:  don't mix signed and unsigned types (especially on RHS).
	 */

	ufirst = 90;
	second = 99;
	lsec = ufirst - second;	/* second becomes unsigned and wraps around backward! */

	printf("ufirst(%u) - second(%d) = (long int) %ld\n", ufirst, second, lsec);

	first = ufirst - second; /* but if  */

	printf("ufirst(%u) - second(%d) = (int) %d\n", ufirst, second, first);

	exit(0);
}


/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm " fn "; " (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
