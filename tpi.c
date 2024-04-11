#include <stdio.h>
#include <stdlib.h>
#include <float.h>

/*
 * PiValue() calculates pi to long double accuracy.  It’s good to the last bit
 * if the float radix of your C implementation is 2.
 */
static long double
PiValue(void)
{
	long double theSum, lastSum;
	long double indexA, indexB;
	long double theProd;

	indexA = 0.0L;
	indexB = 1.0L;
	theSum = theProd = 2.0L;
	do {
		lastSum = theSum;
		indexB += 2.0L;
		theProd /= indexB;
		theProd *= ++indexA;
		theSum += theProd;
	} while (theSum != lastSum);

	return theSum;
}

/*
 * prints 1600 digits of (Pi * 10^1599)
 *
 * equiv. to:  echo 'scale=1599; 4*a(1)' | bc -l
 */

#ifdef OBFUSCATED
int n=10000,b,c=5600,d,e,f[5602],g;pi1600(){for(;b-c;)f[b++]=n/5;for(;d=0,g=c*2;c-=14,printf("%.4d",e+d/n),e=d%n)for(b=c;d+=f[b]*n,f[b]=d%--g,d/=g--,--b;d*=b);}
#else
static void
pi1600(void)
{
	int n = 10000;
	int b;
	int c = 5600;			/* must be evenly divisble by 14 */
	int d;
	int e = 0;
	int f[5602];
	int g;

	for (b = 0; b < c; b++) {
		f[b] = n / 5;
	}
	while (c != 0) {
		g = c * 2;
		b = c;
		c -= 14;
		d = 0;
		while (b > 0) {
			d += f[b] * n;
			--g;
			f[b] = d % g;
			--b;
			d /= g;
			--g;
			if (b != 0) {
				d *= b;
			}
		}
		printf("%.4d", e + d / n);
		e = d % n;
	}
	printf("\n");
}

int main(void);
int
main(void)
{
	printf("PiLongDouble = %#.*Lg\n", DBL_DIG, PiValue());

#if 0
	pi1600();
#endif

	exit(0);
}
#endif

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm -f " fn "; " (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
