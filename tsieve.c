/*
 *
 * Sift the Two's and Sift the Three's,
 * The Sieve of Eratosthenes.
 * When the multiples sublime,
 * The numbers that remain are Prime.
 *
 *	Anonymous
 *
 * Eratosthenes said:  "Here's how you find the prime numbers, write down all
 * the integers, strike out every other one, take the next integer that has not
 * been struck out, which in this case would be three, so now strike out every
 * third one.  Then take the next one that survived this process which is five,
 * strike out every fifth one; and so on.  And the survivors of this process are
 * the primes."
 */

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum {
	PRIMES_UP_TO = 500,
};

#define round2max(x) (((x) < (double)INTMAX_MIN-0.5 || (x) > (double)INTMAX_MAX+0.5) ? abort(),0 : ((x)>=0?(intmax_t)((x)+0.5):(intmax_t)((x)-0.5)))

bool *eratosthenes(size_t n, size_t *cp);

/*
 * A plain implementation of the sieve, without any real optimizations:
 */
bool *
eratosthenes(size_t n,
	     size_t *cp)
{
	bool *p;
	register size_t i, k, r;

	*cp = n;
	p = malloc(n + 1 * sizeof(*p));

	memset(p, true, n * sizeof(*p)); /* assume all are prime */

	p[0] = false; (*cp)--;		/* zero is not prime */
	p[1] = false; (*cp)--;		/* neither is one! */

	r = (size_t) round2max(sqrt((double) n));
	for (i = 2; i <= r; i++) {
		if (p[i] == true) {
			/* knock out every i-eth one after i... */
			for (k = i * i; k <= n; k += i) {
				/* iff it is not already knocked out! */
				if (p[k] == true) {
					p[k] = false;
					(*cp)--;
				}
			}
		}
	}

	return p;
}

bool *notprimes(size_t n, size_t *cp);

/*
 * A plain implementation of the sieve, which uses calloc() and thus avoids
 * having to re-set the array to ones (but returns with non-primes marked true):
 */
bool *
notprimes(size_t n,
       size_t *cp)
{
	bool *notp;
	register size_t i, k, r;

	*cp = n + 1;
	notp = calloc(n + 1, sizeof(*notp));

	notp[0] = true; (*cp)--;	/* zero is not prime */
	notp[1] = true; (*cp)--;	/* neither is one! */

	r = (size_t) round2max(sqrt((double) n));
	for (i = 2; i <= r; i++) {
		if (notp[i] != true) {
			/* mark non-prime every i-eth one after i... */
			for (k = i * i; k <= n; k += i) {
				/* iff it is not already marked non-prime! */
				if (notp[k] != true) {
					notp[k] = true;
					(*cp)--;
				}
			}
		}
	}

	return notp;
}


int main(void);

int
main()
{
	size_t n, i, count;

	n = PRIMES_UP_TO;

	printf("\nFinding the prime numbers between 1 and %jd...\n\n", (intmax_t) n);

	/*  */ {
		bool *p;

		p = eratosthenes((size_t) n, &count);

		printf("The %jd prime numbers between 1 and %jd are: ", (intmax_t) count, (intmax_t) n);
		for (i = 0; i <= n; i++) {
			if (p[i] == true) {
				printf("%jd, ", (intmax_t) i);
			}
		}
		puts("\n");
		free(p);
	}

	/*  */ {
		bool *notp;

		notp = notprimes((size_t) n, &count);

		printf("The %jd prime numbers between 1 and %jd are: ", (intmax_t) count, (intmax_t) n);
		for (i = 0; i <= n; i++) {
			if (notp[i] != true) {
				printf("%jd, ", (intmax_t) i);
			}
		}
		puts("\n");

		free(notp);
	}

	exit(0);
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm -f " fn "; " (default-value 'compile-command) " LDLIBS=-lm " fn " && ./" fn))
 * End:
 */
