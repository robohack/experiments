#include <sys/cdefs.h>

#include <sys/resource.h>
#include <sys/wait.h>

#include <err.h>
#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* from: src/external/lgpl3/gmp/dist/tune/time.c */

static int speed_option_verbose = 1;

int getrusage_backwards_p(void);

/*
 * Test whether getrusage goes backwards, return non-zero if it does
 * (suggesting it's flawed).
 *
 * On a macintosh m68040-unknown-netbsd1.4.1 getrusage looks like it's
 * microsecond accurate, but has been seen remaining unchanged after many
 * microseconds have elapsed.  It also regularly goes backwards by 1000 to 5000
 * usecs, this has been seen after between 500 and 4000 attempts taking perhaps
 * 0.03 seconds.  We consider this too broken for good measuring.  We used to
 * have configure pretend getrusage didn't exist on this system, but a runtime
 * test should be more reliable, since we imagine the problem is not confined
 * to just this exact system tuple.
 */

int
getrusage_backwards_p(void)
{
	static int result = -1;
	struct rusage  start, prev, next;
	long  d;
	int   i;

	if (result != -1) {
		return result;
	}
	if (getrusage(RUSAGE_SELF, &start) == -1) {
		err(EXIT_FAILURE, "getrusage()");
	}
	next = start;

	result = 0;
	i = 0;
	for (;;) {
		prev = next;
		getrusage(RUSAGE_SELF, &next);

		if (next.ru_utime.tv_sec < prev.ru_utime.tv_sec ||
		    (next.ru_utime.tv_sec == prev.ru_utime.tv_sec &&
		     next.ru_utime.tv_usec < prev.ru_utime.tv_usec)) {
			if (speed_option_verbose)
				printf("getrusage() went backwards (attempt %d: prev %ld.%06ld -> now %ld.%06ld)\n",
				       i,
				       prev.ru_utime.tv_sec, prev.ru_utime.tv_usec,
				       next.ru_utime.tv_sec, next.ru_utime.tv_usec);
			result = 1;
			break;
		}

		/*
		 * minimum of 1000 attempts, then stop after either 0.5 CPU second
		 * or 1000000 attempts, whichever comes first
		 */
		d = 1000000 * (next.ru_utime.tv_sec - start.ru_utime.tv_sec) +
		    (next.ru_utime.tv_usec - start.ru_utime.tv_usec);
		i++;
		if (i > 1000000 || (i > 1000 && d > 500000)) {
			break;
		}
	}

	return result;
}

/*
 * return the number of bits set to one in a value
 *
 * old-fashioned bit-by-bit bit-twiddling....  very slow!
 */
unsigned int count_ul_bits(unsigned long);

unsigned int
count_ul_bits(unsigned long v)
{
	unsigned int c;

	c = 0;
	/*
	 * we optimize away any high-order zero'd bits...
	 */
	while (v) {
		c += (v & 1);
		v >>= 1;
	}

	return c;
}

void print_rusage(char *, struct rusage);

void
print_rusage(char *pref, struct rusage ru)
{
	if (ru.ru_utime.tv_sec)
		printf("%sru_utime.tv_sec = %ld\n", pref, ru.ru_utime.tv_sec);
	if (ru.ru_utime.tv_usec)
		printf("%sru_utime.tv_usec = %ld\n", pref, ru.ru_utime.tv_usec);
	if (ru.ru_stime.tv_sec)
		printf("%sru_stime.tv_sec = %ld\n", pref, ru.ru_stime.tv_sec);
	if (ru.ru_stime.tv_usec)
		printf("%sru_stime.tv_usec = %ld\n", pref, ru.ru_stime.tv_usec);
	if (ru.ru_maxrss)
		printf("%sru_maxrss = %ld\n", pref, ru.ru_maxrss);
	if (ru.ru_ixrss)
		printf("%sru_ixrss = %ld\n", pref, ru.ru_ixrss);
	if (ru.ru_idrss)
		printf("%sru_idrss = %ld\n", pref, ru.ru_idrss);
	if (ru.ru_isrss)
		printf("%sru_isrss = %ld\n", pref, ru.ru_isrss);
	if (ru.ru_minflt)
		printf("%sru_minflt = %ld\n", pref, ru.ru_minflt);
	if (ru.ru_majflt)
		printf("%sru_majflt = %ld\n", pref, ru.ru_majflt);
	if (ru.ru_nswap)
		printf("%sru_nswap = %ld\n", pref, ru.ru_nswap);
	if (ru.ru_inblock)
		printf("%sru_inblock = %ld\n", pref, ru.ru_inblock);
	if (ru.ru_oublock)
		printf("%sru_oublock = %ld\n", pref, ru.ru_oublock);
	if (ru.ru_msgsnd)
		printf("%sru_msgsnd = %ld\n", pref, ru.ru_msgsnd);
	if (ru.ru_msgrcv)
		printf("%sru_msgrcv = %ld\n", pref, ru.ru_msgrcv);
	if (ru.ru_nsignals)
		printf("%sru_nsignals = %ld\n", pref, ru.ru_nsignals);
	if (ru.ru_nvcsw)
		printf("%sru_nvcsw = %ld\n", pref, ru.ru_nvcsw);
	if (ru.ru_nivcsw)
		printf("%sru_nivcsw = %ld\n\n", pref, ru.ru_nivcsw);
}


void child_usage(void);

unsigned int iter = 2000000;

void
child_usage()
{
	char *name = "worker child";
	struct rusage ru;
	unsigned int i;
	int status;
	pid_t pid;
	char pref[(sizeof(int) * CHAR_BIT / 3) + 2 + sizeof("pid# ")];

	pid = fork();

	switch (pid) {
	case 0:
		/* child */

		/* do work */
		for (i = 0; i < iter; i++) {
			count_ul_bits((unsigned long) time((time_t *) NULL));
		}
		if (getrusage(RUSAGE_SELF, &ru))
			err(EXIT_FAILURE, "getrusage()");

		sprintf(pref, "pid#%d ", (int) getpid());
		print_rusage(pref, ru);

		exit(0);

	case -1:
		/* error */
		err(EXIT_FAILURE, "fork()");

	default:
		/* parent */
		if (waitpid(pid, &status, 0) == -1)
			err(EXIT_FAILURE, "waitpid()");

		if (getrusage(RUSAGE_CHILDREN, &ru))
			err(EXIT_FAILURE, "getrusage()");

		print_rusage("child's ", ru);

		break;
	}
	if (!WIFEXITED(status)) {
		if (WIFSIGNALED(status)) {
			errx(EXIT_FAILURE,
			     "%s(%u) failed: killed by SIG%s: %s\n",
			     name,
			     (unsigned int) pid,
			     sys_signame[WTERMSIG(status)],
			     sys_siglist[WTERMSIG(status)]);	/* or strsignal() */
		} else if (WIFSTOPPED(status)) {
			errx(EXIT_FAILURE,
			     "%s(%u) failed: stopped by SIG%s: %s\n",
			     name,
			     (unsigned int) pid,
			     sys_signame[WSTOPSIG(status)],
			     sys_siglist[WSTOPSIG(status)]);	/* or strsignal() */
#if 0
		} else {		/* impossible? */
			fprintf(stderr, "%s(%u) failed: not-exited, not-stopped, status %d\n",
			        name,
			        (unsigned int) pid,
			        status);
#endif
		}
	} else if (WEXITSTATUS(status) != 0) {
		errx(EXIT_FAILURE,
		     "%s(%u) terminated with %d (non-zero) status\n",
		     name,
		     (unsigned int) pid,
		     WEXITSTATUS(status));
	} else {
		fprintf(stderr,
		        "%s(%u) terminated normally\n",
		        name,
		        (unsigned int) pid);
	}
}

int main(void);

int
main()
{
	int r;
	struct rusage ru;

	r = getrusage_backwards_p();

	if (r) {
		puts("getrusage(2) can go backwards!");
	}

	memset(&ru, 0xff, sizeof(ru));

	if (getrusage(RUSAGE_SELF, &ru)) {
		err(EXIT_FAILURE, "getrusage()");
	}

	print_rusage("initial ", ru);

	if (r) {
		child_usage();
	}

	if (getrusage(RUSAGE_SELF, &ru)) {
		err(EXIT_FAILURE, "getrusage()");
	}

	print_rusage("final  ", ru);

	exit(r);
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm -f " fn "; " (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
