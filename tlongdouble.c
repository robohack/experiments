#include <math.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * From: Michael Cheponis <michael.cheponis@gmail.com>
 */


int main(void);

int
main(void)
{
	/*
	 * Compute:  arcsin (arccos(arctan(tan(cos(sin (9))))))
	 *
	 * argument is in degrees
	 * */
	long double pi =
		3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679L;
	long double x,y,z,ans;

	x = pi / 20.0L;			/* 9 degrees is pi/20 */
	y = tanl(cosl(sinl(x)));
	z = asinl(acosl(atanl(y)));

	ans = z * 180.0L / pi;		/* Convert to degrees:  rads * 180/pi */

	printf("9 is %38.36Lf,\nerror is %38.36Lf,\nsizeof(long double)=%ld\n",
	       ans, 9.0L-ans, sizeof(long double));

	exit(0);
}


/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm -f " fn "; " (default-value 'compile-command) " LDLIBS=-lm " fn " && ./" fn))
 * End:
 */
