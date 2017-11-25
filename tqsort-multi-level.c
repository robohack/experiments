#include <stdio.h>
#include <stdlib.h>

struct job {
	int pri;			/* priority level */
	int st;				/* submission time */
	int lrt;			/* last run time */
};

static void
print_jobs(struct job jobs[],
           unsigned int njobs)
{
	unsigned int i;

	for (i = 0; i < njobs; i++) {
		printf("%d, %d, %d\n", jobs[i].pri, jobs[i].st, jobs[i].lrt);
	}

	return;
}

/* as per strcmp() et al... */
enum {
	CMP_LOWER = -1,
	CMP_EQUAL = 0,
	CMP_GREATER = 1,
};

static int
int_cmp(int i1,
        int i2)
{
	if (i1 < i2)
		return CMP_LOWER;
	else if (i1 > i2)
		return CMP_GREATER;
	else
		return CMP_EQUAL;
}

static int
pure_job_cmp(const void *v1,
             const void *v2)
{
	const struct job *j1 = v1, *j2 = v2;

	if (j1->pri != j2->pri) {
		/* first priority is priority... */

		return int_cmp(j1->pri, j2->pri);
	}
	if(j1->st != j2->st) {
		/* then compare their submission times */

		return int_cmp(j1->st, j2->st);
	}
	/* else compare their last-run times */

	return int_cmp(j1->lrt, j2->lrt);
}

/*
 * newest first, then by priority else submission time; or by priority else
 * submission time else oldest (smallest) last run time.
 */
static int
npsf_or_pslf_job_cmp(const void *v1,
                     const void *v2)
{
	const struct job *j1 = v1, *j2 = v2;

	/* if these are both new jobs (no last-run time)... */
	if(j1->lrt == 0 && j2->lrt == 0) {
		if (j1->pri != j2->pri) {
			/* first priority is always priority... */

			return int_cmp(j1->pri, j2->pri);
		}
		/* then compare their submission times */

		return int_cmp(j1->st, j2->st);
	}
	/* else if they are both not new */
	if(j1->lrt != 0 && j2->lrt != 0) {
		if (j1->pri != j2->pri) {
			/* first priority is still priority... */

			return int_cmp(j1->pri, j2->pri);
		}
		/* and they have different submission times */
		if(j1->st != j2->st) {
			/* then compare their submission times */

			return int_cmp(j1->st, j2->st);
		}
	}
	/* else compare their last-run times */

	return int_cmp(j1->lrt, j2->lrt);
}


/* now demonstrate that groups form in priority order with qsort() */

/*
 *	Pure:		NPSForPSLF:
 *	(p,s,l)		(p,s,l)
 *
 *	(1,1,0)		(1,1,0)
 *	(1,2,0)		(1,2,0)
 *
 *	(1,1,1)		(2,1,0)
 *	(1,2,1)		(2,2,0)
 *
 *	(1,1,2)		(1,1,1)
 *	(1,2,2)		(1,2,1)
 *
 *	(2,1,0)		(1,1,2)
 *	(2,2,0)		(1,2,2)
 *
 *	(2,1,1)		(2,1,1)
 *	(2,2,1)		(2,2,1)
 *
 *	(2,1,2)		(2,1,2)
 *	(2,2,2)		(2,2,2)
 */
static
struct job jobs[] =
{
	{ 1, 0, 0, },
	{ 1, 2, 0, },
	{ 1, 1, 0, },
	{ 1, 2, 1, },
	{ 1, 0, 1, },
	{ 1, 1, 1, },
	{ 1, 0, 2, },
	{ 1, 1, 2, },
	{ 1, 2, 2, },
	{ 2, 1, 0, },
	{ 2, 0, 0, },
	{ 2, 2, 0, },
	{ 2, 0, 1, },
	{ 2, 2, 1, },
	{ 2, 1, 1, },
	{ 2, 1, 2, },
	{ 2, 2, 2, },
	{ 2, 0, 2, },
};

int main(int, char **);

int
main(argc, argv)
	int argc;
	char *argv[];
{
	(void) (argc && argv);

	puts("Original:");
	print_jobs(jobs, sizeof(jobs) / sizeof(struct job));

	qsort(jobs, sizeof(jobs) / sizeof(struct job), sizeof(struct job), pure_job_cmp);

	puts("Pure-left-to-right-decreasing-priority:");
	print_jobs(jobs, sizeof(jobs) / sizeof(struct job));

	qsort(jobs, sizeof(jobs) / sizeof(struct job), sizeof(struct job), npsf_or_pslf_job_cmp);

	puts("1st-then-3rd-if-zero-else-2nd-then-3rd:");
	print_jobs(jobs, sizeof(jobs) / sizeof(struct job));

	exit(0);
}


/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm " fn "; " (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
