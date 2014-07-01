#if defined(__SVR4) && defined(__sun)
# define __P(p) p
# include <siginfo.h>
#else  /* assume modern 4.4BSD-based system */
# include <sys/cdefs.h>
# define HAVE_SYS_SIG
#endif
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>

int main __P((int, char *[]));
void wait_child __P((pid_t, const char *));
void got_sigpipe __P((int));
void child __P((void) __dead);

void
wait_child(pid, name)
	pid_t pid;
	const char *name;
{
	int status;

	if (waitpid(pid, &status, 0) == -1) {
		fprintf(stderr, "child(): waitpid(%u) failed: %s\n", (unsigned int) pid, strerror(errno));
		exit(6);
	}
	if (!WIFEXITED(status)) {
		if (WIFSIGNALED(status)) {
#ifdef HAVE_SYS_SIG
			fprintf(stderr,
				"%s(%u) failed: killed by SIG%s: %s\n",
				name,
				(unsigned int) pid,
				sys_signame[WTERMSIG(status)],
				sys_siglist[WTERMSIG(status)]);	/* or strsignal() */
#else
			fprintf(stderr, "%s(%u) failed: killed by signal %d\n",
				name,
				(unsigned int) pid,
				WTERMSIG(status));
#endif
			exit(7);
		} else if (WIFSTOPPED(status)) {
#ifdef HAVE_SYS_SIG
			fprintf(stderr,
				"%s(%u) failed: stopped by SIG%s: %s\n",
				name,
				(unsigned int) pid,
				sys_signame[WSTOPSIG(status)],
				sys_siglist[WSTOPSIG(status)]);	/* or strsignal() */
#else
			fprintf(stderr, "%s(%u) failed: stopped by signal %d\n",
				name,
				(unsigned int) pid,
				WSTOPSIG(status));
#endif
			exit(8);
		} else {		/* impossible? */
			fprintf(stderr, "%s(%u) failed: not-exited, not-stopped, status %d\n",
				name,
				(unsigned int) pid,
				status);
		}
	} else if (WEXITSTATUS(status) != 0) {
		fprintf(stderr, "%s(%u) terminated with %d (non-zero) status\n",
			name,
			(unsigned int) pid,
			WEXITSTATUS(status));
		exit(9);
	} else {
		fprintf(stderr,
			"%s(%u) terminated normally\n",
			name,
			(unsigned int) pid);
	}
}

void
got_sigpipe(sig)
	int sig;
{
	/* XXX fprintf() is (supposedly) not async-signal-safe */
#ifdef HAVE_SYS_SIG
	fprintf(stderr, "Process %u caught SIG%s!\n", (unsigned int) getpid(), sys_signame[sig]);
#else
	fprintf(stderr, "Process %u caught signal %d!\n", (unsigned int) getpid(), sig);
#endif
}

void
child()
{
	int pvect[2];
	pid_t pid;
	char test_str[] = "This is a test written by the first child to the second child.\n";


#ifdef IGNORE_SIGPIPE
	(void) signal(SIGPIPE, SIG_IGN);
#endif
#ifdef CATCH_SIGPIPE
	(void) signal(SIGPIPE, got_sigpipe);
#endif
	if (pipe(pvect) < 0) {
		fprintf(stderr, "pipe() failed: %s\n", strerror(errno));
		exit(1);
	}
	if ((pid = fork()) < 0) {
		fprintf(stderr, "fork() failed: %s\n", strerror(errno));
		exit(2);
	}
	if (pid == 0) {			/* if in the child... */
		if (dup2(pvect[0], 0) < 0) {	/* connect read vector to child's stdin */
			fprintf(stderr, "dup2() failed: %s\n", strerror(errno));
			_exit(3);
		}
		(void) close(pvect[0]);		/* close parent's pipe descriptors */
		(void) close(pvect[1]);
		execl("/usr/bin/true", "true", (char *) NULL);
		fprintf(stderr, "execl(/usr/bin/true) failed: %s\n", strerror(errno));
		_exit(4);		/* just the child... */
	}
	sleep(1);			/* make sure the child has started.... */
	/*
	 * A pipe whose read or write end has been closed is considered widowed.
	 * Writing on such a pipe causes the writing process to receive a SIGPIPE
	 * signal.  Widowing a pipe is the only way to deliver end-of-file to a
	 * reader: after the reader consumes any buffered data, reading a widowed
	 * pipe returns a zero count.
	 */
	if (close(pvect[0]) < 0) {	/* close the read end... */
		fprintf(stderr, "close(pvect[0]) failed: %s\n", strerror(errno));
		exit(2);
	}
	if (write(pvect[1], test_str, sizeof(test_str) - 1) < 0) {
		fprintf(stderr, "write() failed: %s\n", strerror(errno));
		exit(5);
	}
	if (close(pvect[1]) < 0) {
		fprintf(stderr, "close(pvect[1]) failed: %s\n", strerror(errno));
		exit(2);
	}
	wait_child(pid, "second child");

	exit(0);
}

int
main(int argc,
     char *argv[])
{
	char *argv0 = "progname";
#ifndef TEST_SHELL
	int pid;
#endif

	(void) argc;
	argv0 = (argv0 = strrchr(argv[0], '/')) ? argv0 + 1 : argv[0];

#ifndef TEST_SHELL
	/*
	 * we need to do our test inside a child process to avoid the influence
	 * of any shell over the exit status....
	 */
	if ((pid = fork()) < 0) {
		fprintf(stderr, "fork() failed: %s\n", strerror(errno));
		exit(1);
	}
	if (pid == 0)			/* if in the child... */
		child();
	wait_child(pid, "first child");

	exit(0);
#else
	child();
#endif
	/* NOTREACHED */
}
