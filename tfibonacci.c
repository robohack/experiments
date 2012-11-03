#include <sys/resource.h>
#include <sys/time.h>

#include <assert.h>
#include <err.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

/*
 *
 * With 64-bit return values these implementations overflow between 93 and 94
 *
 * Fibonacci(93) ->  12200160415121876738
 * 2^64          ->  18446744073709551616
 * Fibonacci(94) ->  19740274219868223167
 */

/*
 * First the "obvious" tree recursion solution.  This algorithm is
 * excruciatingly slow (it will take exponential time to run as n increases)
 * because for every iteration there will be two recursive calls and the
 * recursion tree shows that for every n, recursive calls will be made to
 * recalculate every value of fibonacci(n - 1) and fibonacci(n - 2), each
 * through to fibonacci(0).
 *
 * 38hrs 22mins CPU time and counting on the fib(93) problem on my iMac!
 *
 * This first example is very ugly and wasteful looking....
 *
 * however for some compilers it may actually be faster than the three
 * apparently simpler tree-recursion variants which follow...
 */
long long unsigned int fibRec1(unsigned int);

long long unsigned int
fibRec1(unsigned int n)
{
	if (n < 2) {
		return n;
	} else {
		unsigned int i;
		long long unsigned int f[2];

		n--;
		/*
		 * equivalent to:
		 *
		 *	f[0] = fibRec1(n);
		 *	f[1] = fibRec1(n - 1);
		 */
		for (i = 0; i < 2; i++) {
			f[i] = fibRec1(n - i);
		}

		return f[0] + f[1];
	}
}


/*
 * somewhat better looking tree recursion implemenation
 */
long long unsigned int fibRec2(unsigned int);

long long unsigned int
fibRec2(unsigned int n)
{
	if (n < 2) {
		return n;
	} else {
		return fibRec2(n - 1) + fibRec2(n - 2);
	}
}


/*
 * alternate clean and simple tree recursion implemenation using the
 * conditional operator....
 */
long long unsigned int fibRec3(unsigned int);

long long unsigned int
fibRec3(unsigned int n)
{
	return (n < 2) ? n : (fibRec3(n - 1) + fibRec3(n - 2));
}


/*
 * iteration is always faster than any of the above  :-)
 */
long long unsigned int fibIter(unsigned int);

long long unsigned int
fibIter(unsigned int n)
{
	long long unsigned int last = 0, this = 1, new;
	unsigned int i;

	if (n < 2) {
		return n;
	}
	for (i = 1 ; i < n ; ++i) {
		new = last + this;
		last = this;
		this = new;
	}

	return this;
}

/*
 * iteration with crazy pointer and an array for the three remembered values
 */
long long unsigned int fibIter2(unsigned int);

long long unsigned int
fibIter2(unsigned int n)
{
	long long unsigned int a[3];
	long long unsigned int *p=a;
	unsigned int i;

	for (i = 0; i <= n; ++i) {
		if (i < 2) {
			*p=i;
		} else {
			if (p==a) {
				*p = *(a+1) + *(a+2);
			} else if (p == a+1) {
				*p = *a + *(a+2);
			} else {
				*p = *a + *(a+1);
			}
		}
		if (++p > a+2) {
			p = a;
		}
	}

	return (p == a) ? *(p+2) : *(p-1);
}

/*
 * linear tail-end recursion is almost as fast as iteration...
 *
 * In this approach, no trees are being created, i.e. the function isn't being
 * called twice for every value, therefore the efficiency is much greater.
 *
 * Indeed this is described as the "iterative" version in "Structure and
 * Interpretation of Computer programs".
 */
long long unsigned int fibLinRec(unsigned int);
static long long unsigned int fibInner(long long unsigned int, long long unsigned int, unsigned int);

long long unsigned int
fibLinRec(unsigned int n)
{
	return fibInner(0LLU, 1LLU, n);
}

static long long unsigned int
fibInner(long long unsigned int twoBack,
	 long long unsigned int oneBack,
	 unsigned int n)
{
	if (n == 0) {			/* Used only for fibInner(0, 1, 0) */
		return twoBack;
	} else if (n == 1) {		/* Used only for fibInner(0, 1, 1) */
		return oneBack;
	} else if (n == 2) {
		/*
		 * When the 0 and 1 clauses are included, this clause merely
		 * stops one additional recursion from occurring.
		 */
		return oneBack + twoBack;
	} else {
		/*
		 * oneBack becomes twoBack, and the new oneBack is calculated
		 */
		return fibInner(oneBack, oneBack + twoBack, n - 1);
	}
}

/*
 * Iteration using exponentiation by squaring
 *
 * XXX is this an iterative version of EWD654?
 */
long long unsigned int fibSq(unsigned int);

long long unsigned int
fibSq(unsigned int n)
{
	long long unsigned int i = n - 1, a = 1, b = 0, c = 0, d = 1, t;

	if (n == 0)
		return 0;

	while (i > 0) {
		if (i % 2 == 1) {
			t = d * (b + a) + c * b;
			a = d * b + c * a;
			b = t;
		}
		t = d * (2 * c + d);
		c = c * c + d * d;
		d = t;
		i = i / 2;
	}

	return a + b;
}

/*
 * Alternate iteration using exponentiation by squaring
 *
 * XXX is this an iterative version of EWD654?
 *
 * (may be the fastest of all so far?)
 */
long long unsigned int fibSq2(unsigned int);

long long unsigned int
fibSq2(unsigned int n)
{
	long long unsigned int i = n - 1, a = 1, b = 0, c = 0, d = 1, t;

	if (n == 0)
		return 0;

	while (i > 0) {
		while (i % 2 == 0) {
			t = d * (2 * c + d);
			c = c * c + d * d;
			d = t;
			i = i / 2;
		}
		t = d * (b + a) + c * b;
		a = d * b + c * a;
		b = t;
		i--;
	}

	return a + b;
}


/*
 * Fibonacci numbers using Edsger Dijkstra's algorithm and tail-end recursion
 *
 * http://www.cs.utexas.edu/users/EWD/ewd06xx/EWD654.PDF
 */
long long unsigned int fibEWD654(unsigned int);
static long long unsigned int fibEWD654_half(long long unsigned int, long long unsigned int, long long unsigned int, long long unsigned int, unsigned int);

long long unsigned int
fibEWD654(unsigned int n)
{
	return fibEWD654_half(1LLU, 0LLU, 0LLU, 1LLU, n);
}

static long long unsigned int
fibEWD654_half(long long unsigned int a,
	       long long unsigned int b,
	       long long unsigned int p,
	       long long unsigned int q,
	       unsigned int n)
{
	if (n == 0) {
		return b;
	}

	if (n % 2 == 0) {
		return fibEWD654_half(a,
				      b,
				      (p * p) + (q * q),
				      (q * q) + (2 * p * q),
				      n / 2);
	} else {
		return fibEWD654_half((b * q) + (a * q) + (a * p),
				      (b * p) + (a * q),
				      p,
				      q,
				      n - 1);
	}
}


/*
 * Scheme implementation of....

;;; Fibonacci numbers using Edsger Dijkstra's algorithm
;;; http://www.cs.utexas.edu/users/EWD/ewd06xx/EWD654.PDF

(define (fib n)
  (define (fib-aux a b p q count)
    (cond ((= count 0) b)
          ((even? count)
           (fib-aux a
                    b
                    (+ (* p p) (* q q))
                    (+ (* q q) (* 2 p q))
                    (/ count 2)))
          (else
           (fib-aux (+ (* b q) (* a q) (* a p))
                    (+ (* b p) (* a q))
                    p
                    q
                    (- count 1)))))
  (fib-aux 1 0 0 1 n))

 *
 */

/*
 * C# implementation of....
 *
 * Fibonacci numbers using Edsger Dijkstra's algorithm
 * http://www.cs.utexas.edu/users/EWD/ewd06xx/EWD654.PDF

public static Func Dijkstra = n =>
{
     if( n < 2 )
          return n;

     double half = ( n % 2 == 0 ) ? n / 2 : ( n / 2 ) + 1;

     double p1 = Dijkstra( half );
     double p2 = Dijkstra( half - 1 );
     double result = default( double );

     if( n % 2 == 0 )
          result = ( 2 * p2 + p1 ) * p1;
     else
          result = Math.Pow( p2, 2 ) + Math.Pow( p1, 2 );

     return result;
};

 *
 */

/*
 * Go implementation of....
 *
 * Fibonacci numbers using Edsger Dijkstra's algorithm
 * http://www.cs.utexas.edu/users/EWD/ewd06xx/EWD654.PDF

import (
	"math/big"
)

// recursive splitting Fibonacci evaluation
//   RIP EWD: http://www.cs.utexas.edu/users/EWD/ewd06xx/EWD654.PDF
func fib_half(n int) (*big.Int, *big.Int) {
	if n <= 0 {
		zero := big.NewInt(0)
		return zero, zero
	}
	if n == 1 {
		return big.NewInt(0), big.NewInt(1)
	}

	h := n / 2                      // compute F(n) from F(n/2) for O(log2 n) complexity
	f1, f2 := fib_half(h)           // f1 = F(h-1), f2 = F(h)
	f3 := big.NewInt(0).Add(f1, f2) // f3 = F(h+1)

	t := big.NewInt(0).Add(f1, f3)
	t = t.Mul(t, f2)
	s2 := big.NewInt(0).Mul(f2, f2) // F(h)**2

	if n&1 == 1 { // handle odd n
		s3 := big.NewInt(0).Mul(f3, f3) // F(h+1)**2
		return t, s2.Add(s2, s3)        // (f1 + f3)*f2, f2*f2 + f3*f3
	} // else // handle even n

	s1 := big.NewInt(0).Mul(f1, f1) // F(h-1)**2

	return s1.Add(s1, s2), t        // f1*f1 + f2*f2, (f1 + f3)*f2
}

func fib(n int) *big.Int {
	_, f := fib_half(n)
	return f
}

 *
 */


/*
 * choose your weapon from above!
 *
 * fibRec1, fibRec2, fibRec3,
 * fibIter, fibIter2,
 * fibLinRec,
 * fibSq, fibSq2
 * fibEWD654
 */
#define Fibonacci(N)	fibIter2(N)


/*
 * microtime() - return number of microseconds since some epoch
 *
 * the particular epoch is irrelevant -- we just use the difference between two
 * of these samples taken sufficiently far appart enough that the resolution is
 * also relatively unimportant, though better than 1 second is expected....
 *
 * Note: suseconds_t is for signed values of times in microseconds, and it was
 * first added to POSIX 1003.1 in System Interfaces and Headers, Issue 5
 * published in 1997.  It must be no greater in size than a long int.  Note too
 * that POSIX is a bit finicky in specifying that suseconds_t only needs to
 * hold integers in the range of [0, 1000000] implicitly limiting it to just
 * one second intervals.  However we will abuse it slightly and assume it is at
 * least 32-bits and so can give us at least 35 second intervals, which should
 * be long enough for all our tests?
 */
suseconds_t microtime(void);
void check_clock_res(void);

/*
 * Timing anomalies
 *
 * time(1) uses gettimeofday() to show the "real" time, by which it means the
 * wall-clock time it took to run the process, including the time to do the
 * vfork() and execvp(), ignore some signals, and call wait4().
 *
 * However currently on NetBSD we can see getrusage() report a total of system
 * plus user time of as much as 0.06 seconds longer than gettimeofay() says it
 * took for the whole thing!  E.g.:
 *
 * $ /usr/bin/time -p false
 * real         0.00
 * user         0.03
 * sys          0.03
 *
 * Furthermore gettimeofday() can wander, e.g. due to NTP, or worse.
 *
 * So, we use clock_gettime(CLOCK_MONOTONIC, tspec) instead (if possible)!  It
 * is defined by POSIX 1b (IEEE Std 1003.1b-1993).
 */

#ifdef CLOCK_MONOTONIC

suseconds_t
microtime()
{
	struct timespec tsnow;

	(void) clock_gettime(CLOCK_MONOTONIC, &tsnow);

	return (suseconds_t) ((tsnow.tv_sec * 1000000) + (tsnow.tv_nsec / 1000));
}

void
check_clock_res()
{
	struct timespec res;

	/* XXX "#ifdef CLOCK_PROCESS_CPUTIME_ID"??? */
	if (clock_getres(CLOCK_MONOTONIC, &res) == -1) {
		err(EXIT_FAILURE, "clock_getres(CLOCK_MONOTONIC)");
	}
	warnx("using CLOCK_MONOTONIC timer with resolution: %ld s, %ld ns", res.tv_sec, res.tv_nsec);

	warnx("maximum microtime() interval: %ld s (# of uSecs that fit in %d bits)",
	      ((1 << ((sizeof(suseconds_t) * CHAR_BIT) - 1)) / 1000000U) / 60U,
	      (sizeof(suseconds_t) * CHAR_BIT) - 1);
}

#else /* !CLOCK_MONOTONIC */

/*
 * XXX this is currently for Darwin/Mac OS X, which does not implement the
 * POSIX (IEEE Std 1003.1b-1993) clock_gettime() API
 *
 * Note that on OS X the gettimeofday() function is implemented in libc as a
 * wrapper to either the _commpage_gettimeofday() function, if available, or
 * the normal system call.  If using the COMMPAGE helper then gettimeofday()
 * simply returns the value stored in the COMMPAGE and thus can execute without
 * a context switch.
 */

suseconds_t
microtime()
{
	struct timeval tvnow;

	(void) gettimeofday(&tvnow, (void *) NULL);

	return (suseconds_t) ((tvnow.tv_sec * 1000000) + tvnow.tv_usec);
}
void
check_clock_res()
{
	warnx("maximum microtime() interval: %ld s (# of uSecs that fit in %d bits)",
	      ((1 << ((sizeof(suseconds_t) * CHAR_BIT) - 1)) / 1000000U) / 60U,
	      (sizeof(suseconds_t) * CHAR_BIT) - 1);

	return;
}

#endif /* CLOCK_MONOTONIC */

/* XXX see also timevalsub() */
suseconds_t difftval(struct timeval, struct timeval);

suseconds_t
difftval(struct timeval tstart, struct timeval tend)
{
	tend.tv_sec -= tstart.tv_sec;
	tend.tv_usec -= tstart.tv_usec;

	while (tend.tv_usec < 0) {
		tend.tv_sec--;
		tend.tv_usec += 1000000;
	}
	while (tend.tv_usec >= 1000000) {
		tend.tv_sec++;
		tend.tv_usec -= 1000000;
	}

	return (suseconds_t) ((tend.tv_sec * 1000000) + tend.tv_usec);
}

void print_rusage(struct rusage);

void
print_rusage(struct rusage ru)
{
	if (ru.ru_utime.tv_sec)
		printf("ru_utime.tv_sec = %ld\n", ru.ru_utime.tv_sec);
	if (ru.ru_utime.tv_usec)
		printf("ru_utime.tv_usec = %ld\n", (long int) ru.ru_utime.tv_usec);
	if (ru.ru_stime.tv_sec)
		printf("ru_stime.tv_sec = %ld\n", ru.ru_stime.tv_sec);
	if (ru.ru_stime.tv_usec)
		printf("ru_stime.tv_usec = %ld\n", (long int) ru.ru_stime.tv_usec);
	if (ru.ru_maxrss)
		printf("ru_maxrss = %ld\n", ru.ru_maxrss);
	if (ru.ru_ixrss)
		printf("ru_ixrss = %ld\n", ru.ru_ixrss);
	if (ru.ru_idrss)
		printf("ru_idrss = %ld\n", ru.ru_idrss);
	if (ru.ru_isrss)
		printf("ru_isrss = %ld\n", ru.ru_isrss);
	if (ru.ru_minflt)
		printf("ru_minflt = %ld\n", ru.ru_minflt);
	if (ru.ru_majflt)
		printf("ru_majflt = %ld\n", ru.ru_majflt);
	if (ru.ru_nswap)
		printf("ru_nswap = %ld\n", ru.ru_nswap);
	if (ru.ru_inblock)
		printf("ru_inblock = %ld\n", ru.ru_inblock);
	if (ru.ru_oublock)
		printf("ru_oublock = %ld\n", ru.ru_oublock);
	if (ru.ru_msgsnd)
		printf("ru_msgsnd = %ld\n", ru.ru_msgsnd);
	if (ru.ru_msgrcv)
		printf("ru_msgrcv = %ld\n", ru.ru_msgrcv);
	if (ru.ru_nsignals)
		printf("ru_nsignals = %ld\n", ru.ru_nsignals);
	if (ru.ru_nvcsw)
		printf("ru_nvcsw = %ld\n", ru.ru_nvcsw);
	if (ru.ru_nivcsw)
		printf("ru_nivcsw = %ld\n", ru.ru_nivcsw);
}

int main(int, char *[]);

int
main(int argc,
     char *argv[])
{
	unsigned int i;
	struct rusage tru_s;
	struct rusage ru_e;
	suseconds_t walltm_s;
	suseconds_t walltm_e;

	check_clock_res();
	walltm_s = microtime();

	if (getrusage(RUSAGE_SELF, &tru_s)) {
		err(EXIT_FAILURE, "getrusage()");
	}

	for (i = 1; i < (unsigned int) argc; i++) {
		unsigned int n;
		struct rusage ru_s;
		suseconds_t incrtm_s = microtime();
		suseconds_t incrtm_e;

		if (getrusage(RUSAGE_SELF, &ru_s)) {
			err(EXIT_FAILURE, "getrusage()");
		}

		if (sscanf(argv[i], "%u", &n) == 1) {
			printf("Fibonacci(%u) -> %llu\n", n, Fibonacci(n));
		}

		incrtm_e = microtime();
		if (getrusage(RUSAGE_SELF, &ru_e)) {
			err(EXIT_FAILURE, "getrusage()");
		}

		putchar('\n');
		printf("user   CPU (uSec): %d\n", difftval(ru_s.ru_utime, ru_e.ru_utime));
		printf("system CPU (uSec): %d\n", difftval(ru_s.ru_stime, ru_e.ru_stime));
		printf("total  CPU (uSec): %d\n", (difftval(ru_s.ru_utime, ru_e.ru_utime) +
						   difftval(ru_s.ru_stime, ru_e.ru_stime)));
		printf("wall time  (uSec): %d\n", incrtm_e - incrtm_s);
		putchar('\n');
	}

	/* include times for the last set of printf()s too... */
	walltm_e = microtime();
	if (getrusage(RUSAGE_SELF, &ru_e)) {
		err(EXIT_FAILURE, "getrusage()");
	}

	print_rusage(ru_e);
	putchar('\n');

	printf("user   CPU (uSec): %d\n", difftval(tru_s.ru_utime, ru_e.ru_utime));
	printf("system CPU (uSec): %d\n", difftval(tru_s.ru_stime, ru_e.ru_stime));
	printf("total  CPU (uSec): %d\n", (difftval(tru_s.ru_utime, ru_e.ru_utime) +
					   difftval(tru_s.ru_stime, ru_e.ru_stime)));

	printf("total wall (uSec): %d\n\n", walltm_e - walltm_s);

	exit(0);
}
