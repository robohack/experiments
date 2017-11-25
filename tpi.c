#ifdef OBFUSCATED
int n=10000,b,c=5600,d,e,f[5602],g;main(){for(;b-c;)f[b++]=n/5;for(;d=0,g=c*2;c-=14,printf("%.4d",e+d/n),e=d%n)for(b=c;d+=f[b]*n,f[b]=d%--g,d/=g--,--b;d*=b);}
#else

#include <stdio.h>
#include <stdlib.h>

/*
 * prints 1600 digits of (Pi * 10^1599)
 *
 * equiv. to:  echo 'scale=1599; 4*a(1)' | bc -l
 */

int main(void);
int
main()
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

	exit(0);
}

#endif

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm " fn "; " (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
