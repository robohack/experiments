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
 * excruciatingly slow as n grows.  Indeed it will take exponential time to run
 * as n increases because, as the recursion tree shows, for every n recursive
 * calls will be made to recalculate every value of fibonacci(n - 1) and
 * fibonacci(n - 2), each through to fibonacci(0).  I.e. the number of times
 * that fibonacci(0) is computed is the same as fibonacci(n + 1)!
 *
 * If one call takes 1ns to complete then fibonacci(93) could take nearly 626
 * years to run.
 *
 * This first example is extremely ugly and wasteful, but has actually been
 * seen online as someone's proposed solution!
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
	if (n >= 2) {
		return fibRec2(n - 1) + fibRec2(n - 2);
	} else {
		return n;
	}
}


/*
 * somewhat better looking tree recursion implemenation
 */
long long unsigned int fibRec3(unsigned int);

long long unsigned int
fibRec3(unsigned int n)
{
	if (n < 2) {
		return n;
	} else {
		return fibRec3(n - 1) + fibRec3(n - 2);
	}
}


/*
 * alternate clean and simple tree recursion implemenation using the
 * conditional operator....
 */
long long unsigned int fibRec4(unsigned int);

long long unsigned int
fibRec4(unsigned int n)
{
	return (n < 2) ? n : (fibRec4(n - 1) + fibRec4(n - 2));
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
 * iteration with a crazy pointer and an array for the three remembered values
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
 * linear tail-end recursion is almost as, or as fast as iteration...
 *
 * In this approach, no trees are being created, and there is only one
 * recursive call for every value of 0 ... n.
 *
 * Some compilers will spot tail-end recursion and optimize it into iteration.
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
 * choose your weapon from above!
 *
 * fibRec1, fibRec2, fibRec3, fibRec4,
 * fibIter, fibIter2,
 * fibLinRec,
 * fibEWD654
 * fibSq, fibSq2
 */
#define Fibonacci	fibRec2

/*
 * local C89-only versions of <sys/cdefs.h>'s ___STRING() and __STRING(),
 * respectively:
 *
 * The former is needed for use in a nested macro
 */
#define STRINGIFY_SYMBOL(s)	STRINGIFY_MACRO(s)
#define STRINGIFY_MACRO(s)	#s

/*
 * microtime() - return number of microseconds since some epoch
 *
 * the particular epoch is irrelevant -- we just use the difference between two
 * of these samples taken sufficiently far appart enough that the resolution is
 * also relatively unimportant, though better than 1 second is expected....
 */
static u_quad_t microtime(void);
static void check_clock_res(void);

/*
 * Timing anomalies
 *
 * time(1) uses gettimeofday() to show the "real" time, by which it means the
 * wall-clock time it took to run the process, including the time to do the
 * vfork() and execvp(), ignore some signals, and call wait4().
 *
 * However currently on NetBSD because of the bogus way 4BSD has aproximately
 * always divied up time between user time and system tiem we can see
 * getrusage() report a total of system plus user time of as much as 0.06
 * seconds longer than gettimeofay() says it took for the whole thing!  E.g.:
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

static u_quad_t
microtime()
{
	struct timespec tsnow;

	(void) clock_gettime(CLOCK_MONOTONIC, &tsnow);

	return (u_quad_t) ((tsnow.tv_sec * 1000000) + (tsnow.tv_nsec / 1000));
}

static void
check_clock_res()
{
	struct timespec res;

	/* XXX "#ifdef CLOCK_PROCESS_CPUTIME_ID"??? */
	if (clock_getres(CLOCK_MONOTONIC, &res) == -1) {
		err(EXIT_FAILURE, "clock_getres(CLOCK_MONOTONIC)");
	}
	warnx("using CLOCK_MONOTONIC timer with resolution: %ld s, %ld ns", (long) res.tv_sec, res.tv_nsec);
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

static u_quad_t
microtime()
{
	struct timeval tvnow;

	(void) gettimeofday(&tvnow, (void *) NULL);

	return (u_quad_t) ((tvnow.tv_sec * 1000000) + tvnow.tv_usec);
}
static void
check_clock_res()
{
	return;
}

#endif /* CLOCK_MONOTONIC */


/* XXX see also timevalsub() */
long int difftval(struct timeval, struct timeval);

long int
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

	return (long int) ((tend.tv_sec * 1000000) + tend.tv_usec);
}

void print_rusage(const char *, struct rusage);

#define PRINT_IF(var)	(var) ?  printf("%s%s = %ld\n", pref, STRINGIFY_SYMBOL(var), var) : 0

void
print_rusage(const char *pref,
	     struct rusage ru)
{
	PRINT_IF(ru.ru_utime.tv_sec);
	PRINT_IF(ru.ru_utime.tv_usec);
	PRINT_IF(ru.ru_stime.tv_sec);
	PRINT_IF(ru.ru_stime.tv_usec);
	PRINT_IF(ru.ru_maxrss);
	PRINT_IF(ru.ru_ixrss);
	PRINT_IF(ru.ru_idrss);
	PRINT_IF(ru.ru_isrss);
	PRINT_IF(ru.ru_minflt);
	PRINT_IF(ru.ru_majflt);
	PRINT_IF(ru.ru_nswap);
	PRINT_IF(ru.ru_inblock);
	PRINT_IF(ru.ru_oublock);
	PRINT_IF(ru.ru_msgsnd);
	PRINT_IF(ru.ru_msgrcv);
	PRINT_IF(ru.ru_nsignals);
	PRINT_IF(ru.ru_nvcsw);
	PRINT_IF(ru.ru_nivcsw);
}


#if (__GNUC__ == 4) && (__GNUC_MINOR__ == 1) && (__GNUC_PATCHLEVEL__ == 2) && (__OPTIMIZE__ == 1)
/*
 * XXX this is WONKY!  GCC 4.1.2 re-orders the microtime() and getrusage()
 * calls around the call to the test function, but only with -O2 (not -O3!).
 * Making the result into a global value prevents that mess.  GCC 4.1.3 does
 * not have the same problem.
 *
 * Note we do this explicit test in hopes of exposing other compilers which
 * might do similarly bad re-ordering.
 */
# define RESULT_MUST_BE_GLOBAL
long long unsigned int result;
#endif

#if !defined(__GNUC__) || (defined(__GNUC__) && defined(__STRICT_ANSI__))
# define typeof	__typeof__
#endif

int main(int, char *[]);

int
main(int argc,
     char *argv[])
{
	unsigned int i;
	struct rusage tru_s;
	struct rusage ru_e;
	typeof(microtime()) walltm_s;
	typeof(microtime()) walltm_e;

	walltm_s = microtime();
	if (getrusage(RUSAGE_SELF, &tru_s)) {
		err(EXIT_FAILURE, "getrusage()");
	}

	if (sizeof(microtime()) < sizeof(long long unsigned int)) {
		/*
		 * XXX because the compiler computes the expression above as a
		 * constant at compile time it may warn us that this block will
		 * never be executed.
		 *
		 * We have to ignore the warning because it is impossible to
		 * use sizeof() in the preprocessor (were as otherwise we could
		 * avoid trying to compile this block in the first place).
		 */
		warnx("maximum microtime() interval: %llu seconds (# of uSecs that fit in %d bits)",
		      ((1LLU << ((sizeof(microtime()) * CHAR_BIT) - 1)) / 1000000U) / 60U,
		      (int) (sizeof(microtime()) * CHAR_BIT) - 1);
	}
	check_clock_res();

	printf("calculating Fibonacci numbers using the %s() function...\n", STRINGIFY_SYMBOL(Fibonacci));
	puts("(per-calc wall times don't incl. getrusage() calls, which incl. microtime())\n");

	for (i = 1; i < (unsigned int) argc; i++) {
		unsigned int n;

		if (sscanf(argv[i], "%u", &n) == 1) {
#ifndef RESULT_MUST_BE_GLOBAL
			long long unsigned int result;
#endif
			struct rusage ru_s;
			typeof(microtime()) incrtm_s;
			typeof(microtime()) incrtm_e;

			if (getrusage(RUSAGE_SELF, &ru_s)) {
				err(EXIT_FAILURE, "getrusage()");
			}
			incrtm_s = microtime();

			result = Fibonacci(n);

			incrtm_e = microtime();
			if (getrusage(RUSAGE_SELF, &ru_e)) {
				err(EXIT_FAILURE, "getrusage()");
			}

			printf("%s(%u) -> %llu\n", STRINGIFY_SYMBOL(Fibonacci), n, (long long unsigned int) result);

			putchar('\n');
			printf("  user CPU (uSec): %ld\n", difftval(ru_s.ru_utime, ru_e.ru_utime));
			printf("system CPU (uSec): %ld\n", difftval(ru_s.ru_stime, ru_e.ru_stime));
			printf(" total CPU (uSec): %ld\n", (difftval(ru_s.ru_utime, ru_e.ru_utime) +
			                                    difftval(ru_s.ru_stime, ru_e.ru_stime)));
			printf(" wall time (uSec): %lld\n", (long long int) (incrtm_e - incrtm_s));
			printf(" wait time (uSec): %lld\n", ((long long int) (incrtm_e - incrtm_s) -
			                                     (long long int) ((difftval(ru_s.ru_utime, ru_e.ru_utime) +
			                                                       difftval(ru_s.ru_stime, ru_e.ru_stime)))));
		}
		putchar('\n');
	}

	/* include times for the last set of printf()s too... */
	if (getrusage(RUSAGE_SELF, &ru_e)) {
		err(EXIT_FAILURE, "getrusage()");
	}

	puts("done...\n");

	print_rusage("total ", ru_e);
	putchar('\n');

	printf("  user CPU (uSec): %ld\n", difftval(tru_s.ru_utime, ru_e.ru_utime));
	printf("system CPU (uSec): %ld\n", difftval(tru_s.ru_stime, ru_e.ru_stime));
	printf(" total CPU (uSec): %ld\n", (difftval(tru_s.ru_utime, ru_e.ru_utime) +
	                                    difftval(tru_s.ru_stime, ru_e.ru_stime)));

	walltm_e = microtime();
	printf("total wall (uSec): %lld\n", (long long int) walltm_e - walltm_s);

	printf("total wait (uSec): %lld\n", ((long long int) (walltm_e - walltm_s) -
	                                     (((long long int) difftval(tru_s.ru_utime, ru_e.ru_utime) +
	                                       (long long int) difftval(tru_s.ru_stime, ru_e.ru_stime)))));
	putchar('\n');

	exit(0);
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm " fn "; " (default-value 'compile-command) " CSTD=c99 " fn " && ./" fn))
 * End:
 */
