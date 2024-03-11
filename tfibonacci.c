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

/* XXX see also timevalsub() */
suseconds_t difftval(struct timeval, struct timeval);
suseconds_t
difftval(struct timeval tstart, struct timeval tend)
{
	tend.tv_sec -= tstart.tv_sec;
	tend.tv_usec -= tstart.tv_usec;

	/*
	 * be extremely careful that any over/under "impossible" tv_usec
	 * values from the above subtractions or earlier are ironed out and it
	 * is always left in range.
	 *
	 * XXX instead of a loop we could/should use '/' and '%', if this is
	 * right:
	 *
	 *	if (tend.tv_usec < 0) {
	 *	 	tv_sec -= (abs(tv_usec) / 1000000) + 1;
	 *	 	tv_usec = 1000000 - (abs(tv_usec) % 1000000);
	 *	}
	 *	if (tend.tv_usec >= 1000000) {
	 *	 	tv_sec += tv_usec / 1000000;
	 *	 	tv_usec = tv_usec % 1000000;
	 *	}
	 */
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

/*
 * Timing anomalies
 *
 * time(1) uses gettimeofday() to show the "real" time, by which it means the
 * wall-clock time it took to run the process, including the time to do the
 * vfork() and execvp(), ignore some signals, and call wait4().
 *
 * However currently on NetBSD because of the bogus way 4BSD has approximately
 * always divied up time between user time and system time we can see
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
 * Use the POSIX.1b-1993 clock_gettime(CLOCK_MONOTONIC, tspec) instead if possible!
 *
 * WARNING:  apparently the Linux folks mis-read the POSIX.1b specifications
 * and/or didn't understand the definition of a monotonically increasing time
 * clock, and their CLOCK_MONOTONIC clock is affected by system time
 * adjustments, so you have to use their invented non-standard
 * CLOCK_MONOTONIC_RAW clock to get a real monotonic time clock.  Note too that
 * some sources claim CLOCK_MONOTONIC_RAW sometiems produces garbage results,
 * and is also significantly more expensive to call [1].
 *
 * On the other hand note that CLOCK_MONOTONIC is only subject to incremental
 * corrections, not sudden jumps, so CLOCK_MONOTONIC_RAW would be relevant
 * mainly to cases where more accurate time is wanted over very short intervals,
 * and CLOCK_MONOTONIC would be preferable for longer-term timers measured in
 * minutes, hours or days.  Of couse we _are_ measuring short intervals here.
 *
 * XXX the rest of this is informational -- we really only want CLOCK_MONOTONIC
 * here, though CLOCK_PROCESS_CPUTIME_ID would be more accurate on multitasking
 * systems.
 *
 * POSIX.1b-1999 now says there's a CPT option taken from ISO C:
 *
 *	If _POSIX_CPUTIME is defined, implementations shall support clock ID
 *	values obtained by invoking clock_getcpuclockid(), which represent the
 *	CPU-time clock of a given process.  Implementations shall also support
 *	the special clockid_t value CLOCK_PROCESS_CPUTIME_ID, which represents
 *	the CPU-time clock of the calling process when invoking one of the
 *	clock_*() or timer_*() functions.  For these clock IDs, the values
 *	returned by clock_gettime() and specified by clock_settime() represent
 *	the amount of execution time of the process associated with the clock.
 *	Changing the value of a CPU-time clock via clock_settime() shall have
 *	no effect on the behavior of the sporadic server scheduling policy.
 *
 * (and similar for CLOCK_THREAD_CPUTIME_ID)
 *
 * FreeBSD has these, and also has something similar called CLOCK_PROF, which
 * presumably accounts for all non-idle (non-wait-io) CPU time.
 *
 * The Linux manual warns:
 *
 *	The CLOCK_PROCESS_CPUTIME_ID and CLOCK_THREAD_CPUTIME_ID clocks are
 *	realized on many platforms using timers from the CPUs (TSC on i386,
 *	AR.ITC on Itanium). These registers may differ between CPUs and as a
 *	consequence these clocks may return bogus results if a process is
 *	migrated to another CPU.
 *
 *	If the CPUs in an SMP system have different clock sources then there is
 *	no way to maintain a correlation between the timer registers since each
 *	CPU will run at a slightly different frequency. If that is the case
 *	then clock_getcpuclockid(0) will return ENOENT to signify this
 *	condition. The two clocks will then only be useful if it can be ensured
 *	that a process stays on a certain CPU.
 *
 *	The processors in an SMP system do not start all at exactly the same
 *	time and therefore the timer registers are typically running at an
 *	offset. Some architectures include code that attempts to limit these
 *	offsets on bootup. However, the code cannot guarantee to accurately
 *	tune the offsets. Glibc contains no provisions to deal with these
 *	offsets (unlike the Linux Kernel). Typically these offsets are small
 *	and therefore the effects may be negligible in most cases.
 *
 * The Linux (er, glibc) manual is not clear on whether the CPU TSC registers
 * are saved and restored on each context switch either and there are reports
 * that at least some kernel versions will count the time spent in sleep(3),
 * for example.
 *
 * Apparently Android made it even worse, according to this comment on StkOvf:
 *
 *	For Android users, using CLOCK_MONOTONIC may be problematic since the
 *	app may get suspended, along with the clock.  For that, Android added
 *	the ANDROID_ALARM_ELAPSED_REALTIME timer that is accessible through
 *	ioctl().  [[ Itay Bianco ]]
 *
 * Darwin/MacOS makes things even worse by repeating the Linux mistakes and then
 * also introduces the better performing but less accurate
 * CLOCK_MONOTONIC_RAW_APPROX:
 *
 *	like CLOCK_MONOTONIC_RAW, but reads a value cached by the system at
 *	context switch.  This can be read faster, but at a loss of accuracy as
 *	it may return values that are milliseconds old.
 *
 * Note that FreeBSD has similar CLOCK_*_FAST, e.g. CLOCK_MONOTONIC_FAST, to
 * improve performance but with the limitation of reducing accuracy to "one
 * timer tick".
 *
 * Apparently Linux also fails to adjust CLOCK_MONOTONIC by not necessarily
 * incrementing it while the system is asleep (suspended).  They apparently
 * invented CLOCK_BOOTTIME to work around this sillyness.
 *
 * Note: suseconds_t is for signed values of times in microseconds, and it was
 * first added to POSIX 1003.1 in System Interfaces and Headers, Issue 5
 * published in 1997.  It must be no greater in size than a long int.  Note too
 * that POSIX is a bit finicky in specifying that suseconds_t only needs to
 * hold integers in the range of [0, 1000000] implicitly limiting it to just
 * one second intervals.  However we will abuse it slightly and assume it is at
 * least 32-bits and so can give us at least 35 second intervals, which should
 * be long enough for all our tests?
 *
 * [1] see also:  http://btorpey.github.io/blog/2014/02/18/clock-sources-in-linux/
 */

#ifdef __APPLE__
# define BEST_CLOCK_ID			CLOCK_MONOTONIC
# define BEST_CLOCK_ID_NAME		__STRING(CLOCK_MONOTONIC)
#endif

#if !defined(BEST_CLOCK_ID)
# if defined(CLOCK_MONOTONIC)
#  define BEST_CLOCK_ID			CLOCK_MONOTONIC
#  define BEST_CLOCK_ID_NAME		__STRING(CLOCK_MONOTONIC)
# endif
#endif
/*
 * Note in the above: neither ___STRING(), nor __STRING() can work on
 * BEST_CLOCK_ID to show the intermediate macro's name -- it's all or nothing
 * on expansion of a nested macro definition.
 */

/*
 * microtime() - return number of microseconds since some epoch
 *
 * the particular epoch is irrelevant -- we just use the difference between two
 * of these samples taken sufficiently far appart enough that the resolution is
 * also relatively unimportant, though better than 1 second is expected....
 */
suseconds_t microtime(void);
static void check_clock_res(void);

#if defined(BEST_CLOCK_ID)


# ifdef __APPLE__
#  ifdef __MAC_OS_X_VERSION_MIN_REQUIRED
#    if __MAC_OS_X_VERSION_MIN_REQUIRED < 101200

/*
 * XXX this is for Darwin / Mac OS X prior to Mac OSX 10.12, which did not
 * implement the POSIX (IEEE Std 1003.1b-1993) clock_gettime() API.
 *
 * macOS 10.12 offers CLOCK_MONOTONIC_RAW with the same claims as Linux, but
 * perhaps it is only for Linux compatability and not really necessary.
 *
 * macOS 10.12 also offers CLOCK_MONOTONIC_RAW_APPROX which is apparently
 * implemented using COMMPAGE, so may also be up to milliseconds old.
 *
 * See also:  https://stackoverflow.com/a/21352348/816536
 * and:  https://developer.apple.com/library/content/qa/qa1398/_index.html
 */

#include <sys/time.h>
#include <sys/resource.h>
#include <mach/mach.h>
#include <mach/clock.h>
#include <mach/mach_time.h>
#include <errno.h>
#include <unistd.h>
#include <sched.h>

typedef enum {
	CLOCK_REALTIME,
	CLOCK_MONOTONIC,
	CLOCK_PROCESS_CPUTIME_ID,
	CLOCK_THREAD_CPUTIME_ID
} clockid_t;

static mach_timebase_info_data_t __clock_gettime_inf;

int clock_gettime(clockid_t, struct timespec *);
int
clock_gettime(clockid_t clk_id,
              struct timespec *tp)
{
	kern_return_t	ret;
	clock_serv_t	clk;
	clock_id_t	clk_serv_id;
	mach_timespec_t	tm;
	uint64_t	start, end, delta, nano;
	int		retval = -1;

	switch (clk_id) {
	case CLOCK_REALTIME:
	case CLOCK_MONOTONIC:
		/* XXX these are both the same! */
		/* XXX this is apparently very slow too */
		clk_serv_id = clk_id == CLOCK_REALTIME ? REALTIME_CLOCK : SYSTEM_CLOCK;
		if ((ret = host_get_clock_service(mach_host_self(), clk_serv_id, &clk)) == KERN_SUCCESS) {
			if ((ret = clock_get_time(clk, &tm)) == KERN_SUCCESS) {
				tp->tv_sec  = tm.tv_sec;
				tp->tv_nsec = tm.tv_nsec;
				retval = 0;
			}
		}
		if (KERN_SUCCESS != ret) {
			errno = EINVAL;
			retval = -1;
		}
		break;
	case CLOCK_PROCESS_CPUTIME_ID:
		/*
		 * XXX this is an _extremely_ bad hack, but there you go....
		 *
		 * this measures elapsed time in ticks
		 */
		start = mach_absolute_time();
		if (clk_id == CLOCK_PROCESS_CPUTIME_ID) {
			getpid();
		} else {
			sched_yield();
		}
		end = mach_absolute_time();
		delta = end - start;
		if (__clock_gettime_inf.denom == 0) {
			mach_timebase_info(&__clock_gettime_inf);
		}
		nano = delta * __clock_gettime_inf.numer / __clock_gettime_inf.denom;
		tp->tv_sec = (time_t) (nano * (uint64_t) 1e-9);
		tp->tv_nsec = (long) (nano - (uint64_t) (tp->tv_sec * (time_t) 1e9));
		retval = 0;
		break;
	case CLOCK_THREAD_CPUTIME_ID:
	default:
		errno = EINVAL;
		retval = -1;
	}

	return retval;
}

int clock_getres(clockid_t, struct timespec *);
int
clock_getres(clockid_t clk_id,
             struct timespec *tp)
{
	kern_return_t	ret;
	clock_serv_t	clk;
	clock_id_t	clk_serv_id;
	natural_t	attribute[4];
	int		retval = -1;

	tp->tv_sec = 0;
	tp->tv_nsec = 1;

	switch (clk_id) {
        case CLOCK_REALTIME:
        case CLOCK_MONOTONIC:
		/* XXX these are both the same! */
		clk_serv_id = clk_id == CLOCK_REALTIME ? REALTIME_CLOCK : SYSTEM_CLOCK;
		if ((ret = host_get_clock_service(mach_host_self(), clk_serv_id, &clk)) == KERN_SUCCESS) {
			mach_msg_type_number_t count;

			count = sizeof(attribute)/sizeof(natural_t);
			if ((ret = clock_get_attributes(clk, CLOCK_GET_TIME_RES, (clock_attr_t) attribute, &count )) == KERN_SUCCESS) {
				tp->tv_sec  = 0;
				tp->tv_nsec = attribute[0];
				retval = 0;
			}
		}
		if (KERN_SUCCESS != ret) {
			errno = EINVAL;
			retval = -1;
		}
		break;
	case CLOCK_PROCESS_CPUTIME_ID:
		if (__clock_gettime_inf.denom == 0) {
			mach_timebase_info(&__clock_gettime_inf);
		}
		tp->tv_sec = 0;
		tp->tv_nsec = __clock_gettime_inf.numer / __clock_gettime_inf.denom;
		retval = 0;
		break;
	case CLOCK_THREAD_CPUTIME_ID:
	default:
		errno = EINVAL;
		retval = -1;
	}

	return retval;
}

#   endif
#  endif

# endif /* __APPLE__ */

suseconds_t
microtime(void)
{
	struct timespec tsnow;

	(void) clock_gettime(CLOCK_MONOTONIC, &tsnow);

	return (suseconds_t) ((tsnow.tv_sec * 1000000) + (tsnow.tv_nsec / 1000));
}

static void
check_clock_res(void)
{
	struct timespec res;

	/* XXX "#ifdef CLOCK_PROCESS_CPUTIME_ID"??? */
	if (clock_getres(CLOCK_MONOTONIC, &res) == -1) {
		err(EXIT_FAILURE, "clock_getres(CLOCK_MONOTONIC)");
	}
	warnx("using %s timer with resolution: %ld s, %ld ns", BEST_CLOCK_ID_NAME, res.tv_sec, res.tv_nsec);
}

#else /* ! BEST_CLOCK_ID */

/*
 * XXX N.B.:  apparently on linux times(NULL) is fast and returns a clock_t
 * value of CLK_TKS since the epoch, but it is probably implemented using
 * gettimeofday() anyway... (note: times() is POSIX-1003.1-1990)
 *
 * Note that on OS X the gettimeofday() function is implemented in libc as a
 * wrapper to either the _commpage_gettimeofday() function, if available, or
 * the normal system call.  If using the COMMPAGE helper then gettimeofday()
 * simply returns the value stored in the COMMPAGE and thus can execute without
 * a context switch.
 *
 * On BSD times() is just implemented using getrusage() and gettimeofday().
 */

suseconds_t
microtime(void)
{
	struct timeval tvnow;

	(void) gettimeofday(&tvnow, (void *) NULL);

	return (suseconds_t) ((tvnow.tv_sec * 1000000) + tvnow.tv_usec);
}

static void
check_clock_res(void)
{
	warnx("using gettimeofday() timer with unkown resolution");
	return;
}

#endif /* BEST_CLOCK_ID */

void print_rusage(const char *, struct rusage);

#define PRINT_IF(var)	(var) ?  printf("%s%s = %ld\n", pref, STRINGIFY_SYMBOL(var), (long) var) : 0

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
			printf("  user CPU (uSec): %ld\n", (long int) difftval(ru_s.ru_utime, ru_e.ru_utime));
			printf("system CPU (uSec): %ld\n", (long int) difftval(ru_s.ru_stime, ru_e.ru_stime));
			printf(" total CPU (uSec): %ld\n", (long int) (difftval(ru_s.ru_utime, ru_e.ru_utime) +
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

	printf("  user CPU (uSec): %ld\n", (long int) difftval(tru_s.ru_utime, ru_e.ru_utime));
	printf("system CPU (uSec): %ld\n", (long int) difftval(tru_s.ru_stime, ru_e.ru_stime));
	printf(" total CPU (uSec): %ld\n", (long int) (difftval(tru_s.ru_utime, ru_e.ru_utime) +
						       difftval(tru_s.ru_stime, ru_e.ru_stime)));

	walltm_e = microtime();
	printf("total wall (uSec): %lld\n", (long long int) (walltm_e - walltm_s));

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
