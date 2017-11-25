#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

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

#ifndef static_assert
# define static_assert(expr)						\
	typedef struct {						\
		int STATIC_ASSERT_FLD_NM_(__LINE__): ((expr) ? 1 : -1);	\
	} STATIC_ASSERT_STRUCT_NM_(__LINE__)[(expr) ? 1 : -1]

# define STATIC_ASSERT_FLD_NM_(line)	STATIC_ASSERT_FLD_NM_2_(line)
# if (__STDC__ - 0) > 0
#  define STATIC_ASSERT_FLD_NM_2_(line)	compile_time_assertion_failed_at_line_##line
# else
#  define STATIC_ASSERT_FLD_NM_2_(line)	compile_time_assertion_failed_at_line_/**/line
# endif

# define STATIC_ASSERT_STRUCT_NM_(line)	STATIC_ASSERT_STRUCT_NM_2_(line)
# if (__STDC__ - 0) > 0
#  define STATIC_ASSERT_STRUCT_NM_2_(line) compile_time_assertion_failed_at_line_##line
# else
#  define STATIC_ASSERT_STRUCT_NM_2_(line) compile_time_assertion_failed_at_line_/**/line
# endif
#endif /* !defined(static_assert) */

/*
 * rand_rng() - return a random number in the range 0..N
 *
 * - assuming N is "much less" than RAND_MAX, and also assuming N is less than
 *   UINT_MAX + 1 (the maximum entropy that can be passed to srand())
 *
 * see http://c-faq.com/lib/randrange.html
 *
 * rand() is stupid -- it should have returned unsigned int and taken a range
 * parameter up to RAND_MAX
 *
 * Shifting the range to M..N would require the equivalent of:
 *
 *	M + rand() / (RAND_MAX / (N - M + 1) + 1)
 */
static unsigned int
rand_rng(unsigned int N)
{
	unsigned int x = (RAND_MAX + 1u) / N;
	unsigned long y = x * N;
	unsigned int r;

	/* xxx only needed if RAND_MAX is at or near INT_MAX */
	static_assert(sizeof(unsigned long) > sizeof(unsigned int));

	/* xxx probably only needed if N is "close" to RAND_MAX, and ((RAND_MAX + 1) % N != 0) */
	do {
		r = (unsigned int) rand();
	} while (r >= y);

	return r / x;
}

int main(void);

/*
 * Original idea from:
 * http://www.secufoon.com/how-to-solve-shortest-job-first-scheduling-algorithm-using-c-language/
 */

enum {
	NPIDS = 20,
	MAX_BT = 1000,
};

#define NELEMS(ar)	(sizeof(ar) / sizeof(ar[0]))

int
main()
{
	unsigned int bt[NPIDS], p[NPIDS], wt[NPIDS], tat[NPIDS], i, j, total, pos, temp;
	double avg_wt, avg_tat;

	assert(NELEMS(p) == NELEMS(bt));
	assert(NELEMS(wt) == NELEMS(bt));
	assert(NELEMS(tat) == NELEMS(bt));

	srand(time_seed());

	/* setup random run times */

	printf("\nRun Times:\n");
	for (i = 0; i < NELEMS(bt); i++) {
		bt[i] = rand_rng(MAX_BT);
		printf("PID[%2u] = %5u\n", i + 1, bt[i]);
		p[i] = i + 1;
	}

	// sorting run time in ascending order using selection sort

	for (i = 0; i < NELEMS(bt); i++) {
		for (pos = i, j = i + 1; j < NELEMS(bt); j++) {
			if (bt[j] < bt[pos]) {
				pos = j;
			}
		}
		temp = bt[i];
		bt[i] = bt[pos];
		bt[pos] = temp;
		temp = p[i];
		p[i] = p[pos];
		p[pos] = temp;
	}
	wt[0] = 0;		 // waiting time for first process will be zero

	// calculate waiting time

	for (total = 0, i = 1; i < NELEMS(bt); i++) {
		for (wt[i] = 0, j = 0; j < i; j++) {
			wt[i] += bt[j];
		}
		total += wt[i];
	}
	avg_wt = (double) total / NELEMS(bt);

	printf("\nProcess      Run Time\t Waiting Time\tTurnaround Time\n");
	for (total = 0, i = 0; i < NELEMS(bt); i++) {
		tat[i] = bt[i] + wt[i];	// calculate turnaround time
		total += tat[i];
		printf("PID[%2u]\t\t%5u\t\t%5u\t\t  %5u\n", p[i], bt[i], wt[i], tat[i]);
	}

	avg_tat = (double) total / NELEMS(bt);

	printf("\nAverage Waiting Time = %f\n", avg_wt);
	printf("Average Turnaround Time = %f\n", avg_tat);

	exit(0);
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm " fn "; " (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
