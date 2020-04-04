#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

/* https://stackoverflow.com/q/60743839/816536 */

/*
 * N.B.:   R = A % N is equivalent to R = A – N * (A / N)
 *
 * Where:
 *
 *	R is the remainder
 *	A is the dividend
 *	N is the divisor
 *
 * (see ttimeconvert.c)
 */
bool divisible_ui_p(unsigned int, unsigned int);

int msb(uintmax_t);
int
msb(uintmax_t x)
{
	int mb;

	for (mb = 0; x; mb++) {
		x >>= 1;
	}

	return mb;
}

uintmax_t just_lsb(uintmax_t);
uintmax_t
just_lsb(uintmax_t v)
{
	/*
	 * Given a 2's complement binary integer value x, (x&-x) leaves just
	 * the least significant 1 bit set.  (This was pointed-out by Tom May.)
	 *
	 * The reason this works is that it is equivalent to (x & ((~x) + 1));
	 *
	 * any trailing zero bits in x become ones in ~x, adding 1 to that
	 * carries into the following bit, thus flipping it, and so a logical
	 * AND with the original x yields only the flipped bit... the original
	 * position of the least significant 1 bit.
	 */
	return (v & -v);
}

int lsb(uintmax_t);
int
lsb(uintmax_t v)
{
	return msb(just_lsb(v));
}

int ctz(uintmax_t);
int
ctz(uintmax_t x)
{
	return lsb(x) - 1;
}

#ifdef USE_BY_HAND

/*
 * N.B.:
 *
 *	1. This code is not complete (it doesn't handle even values of n).  This
 *	  can be fixed by adding two comparisons and striping the trailing zeros
 *	  from the divisor (n).
 *
 *	2. The original code was not truly correct (it could result in
 *	   arithmetic overflow for large values of a and n).
 */
bool
divisible_ui_p(unsigned int a,
	       unsigned int n)
{
	uintmax_t ba;

	if (a <= n) {
		if (a == n) {
			return 1;
		}

		return 0;
	}

	ba = a + n;				/* xxx avoid possible overflow of a+n */

#if 0
	/*
	 * __builtin_ctz():  Count Trailing Zeros:  Returns the number of
	 * trailing 0-bits in x, starting at the least significant bit position.
	 * If x is 0, the result is undefined.
	 *
	 * Equivalent to, but faster than: (log_base_2(x ^ -x) - 1), or:
	 *
	 *	for (n = 0, x = (x & -x); x; n++) {
	 *		x >>= 1;
	 *	}
	 *	return n;
	 */
	ba >>= __builtin_ctz(ba);
#else
	ba >>= ctz((uintmax_t) ba);
#endif

	return divisible_ui_p((unsigned int) ba, n);
}
#else

bool
divisible_ui_p(unsigned int a,
	       unsigned int n)
{
    return (a % n == 0);
    //return !!(a % n);
}

#endif

#if defined(USE_RAND) && defined(MESS_WITH_REPRODUCIBILITY)
/*
 * Choose and return an initial random seed based on the current time.
 *
 * From https://benpfaff.org/writings/clc/random-seed.html
 * Based on code by Lawrence Kirby <fred@genesis.demon.co.uk>.
 * getpid() added by "Greg A. Woods" <woods@planix.ca>
 *
 * Usage: srand(time_seed());
 */
static unsigned int
time_seed(void)
{
	time_t timeval;			/* Current time. */
	unsigned char *ptr;		/* Type punned pointed into timeval. */
	unsigned seed;			/* Generated seed. */
	size_t i;

	timeval = time((time_t *) NULL) - (time_t) getpid();
	ptr = (unsigned char *) &timeval;

	seed = 0;
	for (i = 0; i < sizeof(timeval); i++) {
		seed = (seed * (UCHAR_MAX + 2u)) + ptr[i];
	}

	return seed;
}
#endif

/* n.b.:  foo() must not be static so as to prevent optimization */
unsigned int foo(int);

unsigned int
foo(int a)
{
	return (unsigned int) (a + a);
}

int main(void);

int
main()
{
	unsigned int i;
	unsigned int j;

#if defined(USE_RAND) && defined(MESS_WITH_REPRODUCIBILITY)
	srand(time_seed());
#endif

	for (i = 1; i < 100000; i++) {
		for (j = 1; j < 10000; j++) {
			unsigned int a;
			unsigned int b;
			volatile bool r;

#ifdef USE_RAND
			a = (unsigned int) rand() + 1;
			b = (unsigned int) rand();
#else
			a = i;
			b = j;
#endif

			r = divisible_ui_p(b, a);
			i += foo(r);
		}
	}
	exit(0);
}

/*
 * N.B.:  cpuflags from pkgsrc is borked for NetBSD 8.x with GCC-6.5
 *
 * A note about -march= and -mtune=
 *
 * The i386 target for GCC will translate "-mtune=native" -> "-mtune=generic".
 * It will also complain if you try "-mtune=x86-64" and just warn that you could
 * use "-mtune=k8" or "-mtune=generic" as appropriate instead, though if you
 * actually try "-march=x86-64" then it will simply translate that directly to
 * "-mtune=generic".
 */

/*
 * For my old Dell servers with Xeon X5460 "Harpertown" (Yorkfield-based
 * (i.e. "core2", but with the "Penryn" microarchitecture) with models matching
 * "[EX]54??") CPUs:
 *
cpu0: "Intel(R) Xeon(R) CPU           X5460  @ 3.16GHz"
cpu0: Intel Xeon 31xx, 33xx, 52xx, 54xx, Core 2 Quad 8xxx and 9xxx (686-class)
cpu0: family 0x6 model 0x17 stepping 0x6 (id 0x10676)
cpu0: features 0xbfebfbff<FPU,VME,DE,PSE,TSC,MSR,PAE,MCE,CX8,APIC,SEP,MTRR,PGE>
cpu0: features 0xbfebfbff<MCA,CMOV,PAT,PSE36,CLFLUSH,DS,ACPI,MMX,FXSR,SSE,SSE2>
cpu0: features 0xbfebfbff<SS,HTT,TM,SBF>
cpu0: features1 0xce3bd<SSE3,DTES64,MONITOR,DS-CPL,VMX,EST,TM2,SSSE3,CX16,xTPR>
cpu0: features1 0xce3bd<PDCM,DCA,SSE41>
cpu0: features2 0x20100800<SYSCALL/SYSRET,XD,EM64T>
cpu0: features3 0x1<LAHF>
 *
 * compile-command: "rm tdivisibility; make OPTIM='-msse4.1 -march=core2 -O3' CPPFLAGS=-DUSE_RAND tdivisibility && time ./tdivisibility && rm tdivisibility && make OPTIM='-msse4.1 -march=core2 -O3' CPPFLAGS='-DUSE_BY_HAND -DUSE_RAND' tdivisibility && time ./tdivisibility"
 */
/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm " fn "; " (default-value 'compile-command) " " fn " && time ./" fn " && rm " fn " && " (default-value 'compile-command) " CPPFLAGS=-DUSE_BY_HAND " fn " && time ./" fn))
 * End:
 */
