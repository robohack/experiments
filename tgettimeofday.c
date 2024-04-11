#include <sys/cdefs.h>

#include <sys/resource.h>
#include <sys/time.h>

#include <err.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int speed_option_verbose = 1;

int gettimeofday_backwards_p(void);

/*
 * Test whether gettimeofday goes backwards, return non-zero if it does
 */
int
gettimeofday_backwards_p(void)
{
	static int result = -1;
	struct timeval  start, prev, next;
#if 1
	long  d;
#endif
	int   i;

	if (result != -1)
		return result;

	gettimeofday(&start, (void *) NULL);
	next = start;

	result = 0;
	i = 0;
	for (;;) {
		prev = next;
		gettimeofday(&next, (void *) NULL);

		if (next.tv_sec < prev.tv_sec ||
		    (next.tv_sec == prev.tv_sec &&
		     next.tv_usec < prev.tv_usec)) {
			if (speed_option_verbose)
				printf("gettimeofday() went backwards (attempt %d: %ld.%06ld -> %ld.%06ld)\n",
					i,
				       prev.tv_sec, (long) prev.tv_usec,
				       next.tv_sec, (long) next.tv_usec);
			result = 1;
			break;
		}

#if 1
		/*
		 * minimum of 1000 attempts, then stop after either 2.0 CPU second
		 * or 10*10^6 attempts, whichever comes first
		 */
		d = 1000000 * (next.tv_sec - start.tv_sec) +
		    (next.tv_usec - start.tv_usec);
		i++;
		if (i > 10000000 || (i > 1000 && d > 2000000))
			break;
#endif
	}

	return result;
}

static unsigned int
msb(uintmax_t v)
{
        unsigned int mb = 0;

	while (v >>= 1) { /* unroll for more speed...  (see ilog2()) */
		mb++;
	}

        return mb;
}

static unsigned int
ilog10(uintmax_t v)
{
	unsigned int r;
	static unsigned long long int const PowersOf10[] =
		{ 1LLU, 10LLU, 100LLU, 1000LLU, 10000LLU, 100000LLU, 1000000LLU,
		  10000000LLU, 100000000LLU, 1000000000LLU, 10000000000LLU,
		  100000000000LLU, 1000000000000LLU, 10000000000000LLU,
		  100000000000000LLU, 1000000000000000LLU, 10000000000000000LLU,
		  100000000000000000LLU, 1000000000000000000LLU,
		  10000000000000000000LLU };

	if (!v)
		return ~0U;

	/*
	 * By the relationship "log10(v) = log2(v) / log2(10)", we need to
	 * multiply "log2(v)" by "1 / log2(10)", which is approximately
	 * 1233/4096, or (1233, followed by a right shift of 12).
	 *
	 * Finally, since the result is only an approximation that may be off
	 * by one, the exact value is found by subtracting "v < PowersOf10[r]"
	 * from the result.
	 */
	r = ((msb(v) * 1233) >> 12) + 1;

	return r - (v < PowersOf10[r]);
}

/*
 * Convert usec to clock ticks; could do (usec * CLK_TCK) / 1000000,
 * but this would overflow if we switch to nanosec.
 */
#define	CONVTCK(r)	\
	(clock_t)(r.tv_sec * clk_tck + r.tv_usec / (1000000 / clk_tck))


int main(void);

int
main()
{
	struct timeval tvbeg;
	struct timeval tvend;
	struct timezone tz;
	int r;
	int64_t usb;
	int64_t use;
	int64_t usi;
	static long clk_tck;
	unsigned int maxd;
	unsigned int maxc;
	unsigned int maxu;

	memset(&tvbeg, 0x0, sizeof(tvbeg));

	if (gettimeofday(&tvbeg, &tz))
		err(EXIT_FAILURE, "gettimeofday(begin)");

	if (clk_tck == 0)
		clk_tck = sysconf(_SC_CLK_TCK);

	maxd = ilog10(INTMAX_MAX);
	maxc = maxd - ilog10((1000000LLU / (uintmax_t) clk_tck));
	maxu = maxd - ilog10(1000000LLU);

	putchar('\n');
	printf("clk_tck: %jd\n", (intmax_t) clk_tck);

	putchar('\n');
	printf("hit return: ");
	getchar();
	putchar('\n');
	fflush(stdout);

	r = gettimeofday_backwards_p();
	if (r)
		puts("gettimeofday(2) can go backwards!");

	memset(&tvend, 0x0, sizeof(tvend));

	if (gettimeofday(&tvend, &tz))
		err(EXIT_FAILURE, "gettimeofday(end)");

	usb = ((int64_t) tvbeg.tv_sec * 1000000) + tvbeg.tv_usec;
	use = ((int64_t) tvend.tv_sec * 1000000) + tvend.tv_usec;
	usi = use - usb;

	putchar('\n');
	printf("tv_sec:\t\t%*ld s\n", maxu, tvbeg.tv_sec);
	printf("tv_usec:\t%jd us\n", (intmax_t) tvbeg.tv_usec);
	printf("beg:\t\t%*jd us\n", maxd, (intmax_t) usb);
	printf("beg:\t\t%*jd     0x%.16jX clk_tck\n", maxc, (intmax_t) usb * clk_tck / 1000000, (intmax_t) usb * clk_tck / 1000000);
	printf("beg:\t\t%*jd     0x%.16jX clock_t\n", maxc, (intmax_t) CONVTCK(tvbeg), (intmax_t) CONVTCK(tvbeg));


	putchar('\n');
	printf("tv_sec:\t\t%*ld s\n", maxu, tvend.tv_sec);
	printf("tv_usec:\t%jd us\n", (intmax_t) tvend.tv_usec);
	printf("end:\t\t%*jd us\n", maxd, (intmax_t) use);
	printf("beg:\t\t%*jd     0x%.16jX clk_tck\n", maxc, (intmax_t) use * clk_tck / 1000000, (intmax_t) use * clk_tck / 1000000);
	printf("beg:\t\t%*jd     0x%.16jX clock_t\n", maxc, (intmax_t) CONVTCK(tvend), (intmax_t) CONVTCK(tvend));

	putchar('\n');
	printf("elapsed:\t%jd us\n", (intmax_t) usi);


	exit(r);
}


/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm -f " fn "; " (default-value 'compile-command) " " fn " && echo | ./" fn))
 * End:
 */
