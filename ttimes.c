#include <sys/times.h>
#include <limits.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

/* typedef__darwin_clock_tclock_t;  found in types.h */
/* typedef unsigned long __darwin_clock_t  found in _types.h */
int
main(void)
{
	clock_t clk;
	long sc_clk = sysconf(_SC_CLK_TCK);
	struct tms tm;
	double clkf;
	time_t clk2;
	time_t tloc;

	printf("CLK_TCK:              %lu\n", (unsigned long) CLK_TCK);
	printf("sysconf(_SC_CLK_TCK): %lu\n", sc_clk);

	clk = times(&tm);  // clock_t times(struct tms *buffer);

	printf("Output from times():  %jd\n", (intmax_t) clk);

	clkf = (double) clk / CLK_TCK;

	printf("times() / CLK_TCK:    %f\n", clkf);
	if (sc_clk != CLK_TCK) {
		printf("times() / sc_clk:     %f\n", (double) clk / (double) sc_clk);
	}

	clk2 = time(&tloc);
	printf("Output from time():   %jd\n", (intmax_t) clk2);

	exit(0);
}


/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm " fn "; " (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
