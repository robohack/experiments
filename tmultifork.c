#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#if defined(__SVR4) && defined(__sun)
# include <siginfo.h>
#else
	/*
	 * I assume a modern 4.4BSD-based system, or one with a compatlib,
	 * i.e. with sys_signame[] and sys_siglist[]
	 */
#endif

#include <sys/wait.h>

/*
 * Derived from code posted to LinkedIN by Martín Knoblauch Revuelta.
 *
 * multifork() and multiwait() are helper functions for creating a number of
 * child processes and making them run, while ensuring that either:
 *
 *	a) every child is created and gets to continue running
 * or:
 *	b) no one does
 *
 * If fork() fails creating a child process, then the previously created
 * children processes don't get to return from multifork()
 *
 * Note that if a child fails for any reason, including if there's any problem
 * with communicating the start signal to all children, then all we can do is
 * discover the failure(s) using multiwait().
 */

#if 1
# define MAKE_CHILD_READ_FAIL	-1
# define MAKE_RANDOM_CHILD_FAIL	/* defined */
# define MAKE_RANDOM_FORK_FAIL	/* defined */
# define MAKE_RANDOM_WRITE_FAIL	/* defined */
#else
# define MAKE_CHILD_READ_FAIL	/* empty */
#endif

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

int multifork(unsigned int, pid_t []);	       /* make many children */
int multiwait(unsigned int, pid_t [], int []); /* wait for all children */
int run_child(unsigned int);		       /* worker function */

pid_t funny_fork(void);			/* test version (can randomly fail) */
void prognosticate_status(unsigned int, pid_t, int);
int main(void);

#define NUMWORKERS	10U		/* # of worker processes to create */

int
main()
{
	pid_t           pids[NUMWORKERS] = {0};
	int		statuses[NUMWORKERS];
	unsigned int    i;
	int             id;

	srand(time_seed());

	switch ((id = multifork(NUMWORKERS, pids))) {
	case -1:
		fprintf(stderr, "Could not create and start all children.\n");
		exit(EXIT_FAILURE);

	case -2:
		for (i = 0; i < NUMWORKERS; i++) {
			if (pids[i]) {
				fprintf(stderr, "Terminating child[%d]: PID %u\n", i, pids[i]);
				(void) kill(pids[i], SIGTERM);
			}
		}
		(void) multiwait(NUMWORKERS, pids, statuses);
		for (i = 0; i < NUMWORKERS; i++) {
			prognosticate_status(i, pids[i], statuses[i]);
		}

		exit(EXIT_FAILURE);

	case NUMWORKERS:
		printf("Waiting for all childen to terminate...\n");
		(void) multiwait(NUMWORKERS, pids, statuses);

		for (i = 0; i < NUMWORKERS; i++) {
			prognosticate_status(i, pids[i], statuses[i]);
		}
		printf("Done\n");

		exit(EXIT_SUCCESS);

	default:
		/*
		 * each child process will start here, id == child's ID# (not PID!)
		 */
		exit(run_child((unsigned int) id));
	}
}

/*
 * multifork() - start `n' child processes and return their PIDs in
 *
 * A pipe will be used as a form of execution barrier in order to guarantee
 * that either:
 *
 *	a) every child process can by started by fork()
 *
 * or:
 *
 *	b) no child runs and returns to the caller...
 *
 * The return value is:
 *
 * 0..(n-1):
 *	Child process ID# -- its index in the pids[] array
 *
 * n:
 *	Running in parent after successfully signalling all `n' child processes
 *	to start running.
 *
 * -1:
 *	Something went wrong starting processes, likely a failed fork() call.
 *      No child processes should remain even if some were started.
 *
 * -2:
 *	Something went wrong sending the start tokens to the child processes.
 *	Some may have been sent, and so some processes may be running.  Any
 *	non-zero entries in the pids[] array should probably be sent a signal
 *	to tell them to terminate.
 */
int
multifork(unsigned int n,		/* Number of children to create */
	  pid_t *pids)			/* Their process ID's */
{
	int             fd[2];		/* FDs for pipe() to children */
	unsigned int    i;
	char            c;

	if (n < 1 || !pids) {
		errno = EINVAL;
		return -1;
	}

	for (i = 0; i < n; i++)
		pids[i] = 0;

	if (pipe(fd)) {
		int oerrno = errno;

#ifndef NDEBUG
		fprintf(stderr, "multifork(): pipe() failed: %s\n", strerror(errno));
#endif
		errno = oerrno;

		return -1;
	}
	/*
	 * Create n processes
	 */
	for (i = 0; i < n; i++) {

		switch ((pids[i] = funny_fork())) {
			int rc;
			int oerrno;

		case -1:
			oerrno = errno;
			/*
			 * If there is any error then just close both ends of
			 * the pipe.  Any existing child processes will already
			 * be blocked in a read() on the pipe and this will
			 * cause that call in each to return 0, simulating an
			 * EOF on the pipe.
			 */
#ifndef NDEBUG
			fprintf(stderr, "child[%u]: fork() failed: %s\n", i, strerror(errno));
#endif
			(void) close(fd[0]);
			(void) close(fd[1]);

			/*
			 * silently reap any child processes that had been
			 * started but which have now called _exit().
			 */
			(void) multiwait(i, pids, (int *) NULL);

			errno = oerrno;

			return -1;

		case 0:
			/*
			 * Child process:  wait until all other children have
			 * been created by trying to read a character from the
			 * parent's pipe, or until the parent aborts and closes
			 * the pipe causing our read() to fail...
			 *
			 * NOTE:  It is vital to close the child's copy of the
			 * write side file descriptor (fd[1]) of the pipe
			 * _BEFORE_ calling read().  If we don't do this then
			 * the read() will hang forever in the case where the
			 * parent closes the pipe to signal that some problem
			 * occurred and the child processes should not be
			 * started.
			 */
			(void) close(fd[1]);

			/*
			 * Note: even with BSD's better signal semantics,
			 * sigaction() could be used directly to install a
			 * signal handler without SA_RESTART and thus cause the
			 * read() to be interrupted.
			 *
			 * There is potential for EOF on the pipe due to fork()
			 * or other failure in the parent process.  However
			 * other failures should be almost impossible, but as
			 * they say, always expect the unexpected!
			 */
			do {
				errno = 0;
				rc = (int) read(fd[0], &c, (size_t) 1);
			} while (rc != 1 && errno == EINTR);
			if (rc != 1) {
#ifndef NDEBUG
				fprintf(stderr, "child[%u]: PID %u: read(pipe, 1) failed: %s\n",
					i,
					(unsigned int) getpid(),
					rc == 0 ? "EOF on pipe" : strerror(errno));
#endif
				/*
				 * There's nothing we can do here really but
				 * exit as quietly as possible.  There's no
				 * safe and deadlock-free way for all the child
				 * processes to also simultaneously agree on
				 * being ready to continue.
				 *
				 * If we return to the caller then we'd be
				 * returning to code that expects to execute
				 * only ever in the context of the parent
				 * process.  (It could compare it's PID with
				 * the parent process PID, but that just shifts
				 * where the _exit() call should go.)
				 *
				 * Note we must not call exit() either for fear
				 * of running atexit() handlers, etc. set up by
				 * the parent process.  In any case we don't
				 * have to worry about flushing or closing any
				 * STDIO streams because we've not opened any,
				 * and stderr is supposed to be line buffered,
				 * so this is safest.
				 */
				_exit(EXIT_FAILURE);
			}
			close(fd[0]);

			/*
			 * return the child ID# (i.e. its index in pids[])
			 */
			return (int) i;
		}
	}

	/*
	 * Continuing in the parent process -- close our read side of the pipe
	 */
	(void) close(fd[0]);

	/*
	 * All children have been created, and they are waiting.  Let them
	 * continue running now by writing a character (here a '.') for each
	 * one to our pipe's write side.
	 *
	 * XXX we can cause one child to fail here to test some of the error
	 * handling!
	 */
	for (i = 0, c = '.'; i < n MAKE_CHILD_READ_FAIL; i++) {
		int rc;

		/*
		 * Note: even with BSD's better signal semantics, sigaction()
		 * could be used directly to install a signal handler without
		 * SA_RESTART and thus cause the write() to be interrupted.
		 *
		 * Other failures should be impossible, but as they say, always
		 * expect the unexpected!
		 */
		do {
			int r;

			errno = 0;
#ifdef MAKE_RANDOM_WRITE_FAIL
			r = rand();
			if ((r % 11) == 0) {
				errno = ENOTBLK;
				rc = -1;
			} else
#endif
			rc = (int) write(fd[1], &c, (size_t ) 1);
		} while (rc != 1 && errno == EINTR);
		if (rc != 1) {
			/* XXX a return value of 0 should be impossible */
#ifndef NDEBUG
			fprintf(stderr, "write(pipe, 1) #%d failed: %s\n",
				i,
				strerror(errno));
#endif
			(void) close(fd[1]);

			return -2;
		}
	}

	/*
	 * close the write side of the pipe -- if any child did not receive its
	 * startup token for any reason then its read() will return 0 and thus
	 * it will not return to our caller even though we will indicate that
	 * it started successfully.  Its entry in pids[] will be valid though
	 * and its status from multiwait() will indicate that it failed.
	 */
	(void) close(fd[1]);

	return (int) n;
}

int
multiwait(unsigned int n,		/* Number of children */
	  pid_t pids[],			/* Their process ID's */
	  int status[])			/* Their exit status */
{
	unsigned int    i;
	int             ok;

	if (n < 1 || !pids)
		return -1;

	/*
	 * this must wait for the slowest child...
	 */
	for (i = 0, ok = 1; i < n; i++) {
		if (pids[i] == 0)
			continue;
#ifndef NDEBUG
		fprintf(stderr, "waiting for pid[%d]: %u\n", i, (unsigned int) pids[i]);
#endif
		if (waitpid(pids[i], status ? &status[i] : (int *) NULL, 0) != pids[i]) {
			perror("waitpid()");
			ok = 0;
		}
	}

	return ok ? 0 : -1;
}

int
run_child(unsigned int i)
{
	int r;

	printf("I'm child #[%u]: PID %u\n", i, (unsigned int) getpid());
	fflush(stdout);
	/*
	 * process one unit of work....
	 */
	sleep(1);
#ifdef MAKE_RANDOM_CHILD_FAIL
	/* note on good systems the PID itself is always random! */
	srand(time_seed());
	r = rand();
	if ((r % 7) == 0) {
		fprintf(stderr, "child[%u]: trying to cause child failure!\n", i);
		return(EXIT_FAILURE);
	}
#endif
	return(EXIT_SUCCESS);
};

/*
 * funny_fork() --  wrapper around fork()
 *
 * Currently this is used to simulate a random fork() failure.
 *
 * However it could also be used to retry fork() on failure some limited number
 * of times in hopes that the error condition is temporary.
 */
pid_t
funny_fork()
{
	int r;

#ifdef MAKE_RANDOM_FORK_FAIL
	r = rand();
	if ((r % 11) == 0) {
		errno = ECHILD;
		return -1;
	}
#endif
	return fork();
}

void
prognosticate_status(unsigned int id,
		     pid_t pid,
		     int status)
{
	if (!WIFEXITED(status)) {
		if (WIFSIGNALED(status)) {
			fprintf(stderr,
				"child[%u]: PID %u: failed: killed by SIG%s: %s\n",
				id,
				(unsigned int) pid,
				sys_signame[WTERMSIG(status)],
				sys_siglist[WTERMSIG(status)]);
		} else if (WIFSTOPPED(status)) {
			fprintf(stderr,
				"child[%u]: PID %u: failed: stopped by SIG%s: %s\n",
				id,
				(unsigned int) pid,
				sys_signame[WSTOPSIG(status)],
				sys_siglist[WSTOPSIG(status)]);
		} else { /* impossible? */
			fprintf(stderr,
				"child[%u]: PID %u: failed: not-exited, not stopped or signalled, status %d\n",
				id,
				(unsigned int) pid,
				status);
		}
	} else if (WEXITSTATUS(status) != 0) {
		fprintf(stderr,
			"child[%u]: PID %u: terminated with non-zero status == %d\n",
			id,
			(unsigned int) pid,
			WEXITSTATUS(status));
	}
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm -f " fn "; " (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
