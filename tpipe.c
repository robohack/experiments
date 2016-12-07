#if defined(__SVR4) && defined(__sun)
# include <siginfo.h>
#elif defined(__linux__)
# include <sys/cdefs.h>
	/* n.b.:  glibc has no sys_signame[] array, sadly */
#else  /* assume a modern 4.4BSD-based system, or enough compatability */
# include <sys/cdefs.h>
# define HAVE_SYS_SIG
#endif
#if defined(__NetBSD__) || defined(__minix) ||					\
	defined(__FreeBSD__) || defined(__DragonFly__) ||			\
	defined(__APPLE__)
# include <sys/param.h>
#endif
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

/*
 * hmmm.... The *BSDs have had the "wrong" prototype for a long time
 */
#if (defined(__NetBSD__) && (__NetBSD_Version__ - 0 < 699004400)) ||		\
	defined(__FreeBSD__) || defined(__DragonFly__) || defined(__minix) ||	\
	(defined(__APPLE__) && (__DARWIN_UNIX03 - 0 == 0))
typedef unsigned long srandom_seed_t;
#else
typedef unsigned int srandom_seed_t;	/* P1003.1-2008 */
#endif

#ifndef PATH_TRUE
# if defined(__linux__)
#  define PATH_TRUE	"/bin/true"
# else
#  define PATH_TRUE	"/usr/bin/true"
# endif
#endif

#if !defined(__NetBSD__) && !defined(__minix) &&				\
	!(defined(__OpenBSD__) || (defined(__OpenBSD) && (OpenBSD < 201311))) &&\
	!defined(__FreeBSD__) && !defined(__DragonFly__) &&			\
	!defined(__APPLE__)
static const char *argv0;
static void
setprogname(const char *pname)
{
	argv0 = (argv0 = strrchr(pname, '/')) ? argv0 + 1 : pname;
}
static const char *
getprogname(void)
{
	return argv0;
}
#endif

#define DATA_PARENT	"In Xanadu, did Kublai Khan . . ."
#define DATA_CHILD	"A stately pleasure dome decree . . ."

/*
 * This program creates a pair of connected sockets then forks and tries to
 * communicate bi-directionally over them.
 */

int main(int, char *[]);
void wait_child(pid_t, const char *);
void test_a_pipe_fd(const char *, int);
void test_pipe_fds(int [2]);

void
wait_child(pid, name)
	pid_t pid;
	const char *name;
{
	int status;

	if (waitpid(pid, &status, 0) == -1) {
		fprintf(stderr,
		        "child(): waitpid(%u) failed: %s\n",
		        (unsigned int) pid, strerror(errno));
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
			fprintf(stderr,
			        "%s(%u) failed: killed by signal %d\n",
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
			fprintf(stderr,
			        "%s(%u) failed: stopped by signal %d\n",
			        name,
			        (unsigned int) pid,
			        WSTOPSIG(status));
#endif
			exit(8);
		}
#if 0
		else {		/* impossible?  Impossible to know without knowing WIF*() details. */
			fprintf(stderr,
			        "%s(%u) failed: not-exited, not-stopped, status %d\n",
			        name,
			        (unsigned int) pid,
			        status);
		}
#endif
	} else if (WEXITSTATUS(status) != 0) {
		fprintf(stderr,
		        "%s(%u) terminated with %d (non-zero) status\n",
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

static int isapipe(int);

/*
 * from 2.9BSD libc -- a more portable but persnickety version is in GNUlib
 *
 * Linux, in particular, returns ESPIPE for character devices.   Idiots.
 *
 * On other systems ESPIPE may also result from use on a socket
 */
static int
isapipe(f)
	int f;
{
	return(lseek(f, (off_t) 0, SEEK_CUR) < 0 && errno == ESPIPE);
}

void
test_a_pipe_fd(const char *pi,		/* pipe array index */
               int pd)			/* descriptor */
{
	struct stat sb;

	/*
	 * On many BSD-derived systems applying fstat() to a socket (and thus
	 * to a pipe) returns a zeroed buffer, except for the blocksize field,
	 * and (sometimes) a unique device and inode number.
	 *
	 * if (buf.st_mode == 0 && buf.st_size == 0 && buf.st_nlink == 0)
	 *	we_probably_have_a_socket();
	 */
	if (pd < 0) {
		printf("%s: fd[%s] %d invalid\n", getprogname(), pi, pd);
		return;
	}
	if (fstat(pd, &sb) == -1) {
		perror("fstat(pd)");
		return;
	}
	if (S_ISFIFO(sb.st_mode)) {
		printf("%s: fd[%s] %d is a FIFO\n", getprogname(), pi, pd);
	}
	if (S_ISCHR(sb.st_mode)) {
		printf("%s: WARNING: fd[%s] %d is a character device!!!\n", getprogname(), pi, pd);
	}
	if (S_ISBLK(sb.st_mode)) {
		printf("%s: WARNING: fd[%s] %d is a block device!!!\n", getprogname(), pi, pd);
	}
	if (S_ISREG(sb.st_mode)) {
		printf("%s: WARNING: fd[%s] %d is a regular file!!!\n", getprogname(), pi, pd);
	}
	if (S_ISDIR(sb.st_mode)) {
		printf("%s: WARNING: fd[%s] %d is a directory!!!\n", getprogname(), pi, pd);
	}
#ifdef S_ISLNK
	if (S_ISLNK(sb.st_mode)) {
		printf("%s: WARNING: fd[%s] %d is a symbolic link!!!\n", getprogname(), pi, pd);
	}
#endif
#ifdef S_ISSOCK
	if (S_ISSOCK(sb.st_mode)) {
		printf("%s: fd[%s] %d is a socket\n", getprogname(), pi, pd);
	}
#endif
	if (sb.st_nlink == 0) {
		printf("%s: fd[%s] %d has no link count\n", getprogname(), pi, pd);
	} else {
		printf("%s: fd[%s] %d has a link count of %ju\n", getprogname(), pi, pd, (uintmax_t) sb.st_nlink);
	}
	if (sb.st_mode == 0 && sb.st_size == 0 && sb.st_nlink == 0) {
		printf("%s: fd[%s] %d looks like a socket(), or one of a socketpair()\n", getprogname(), pi, pd);
	}
	if (sb.st_dev == 0) {
		printf("%s: fd[%s] %d has no st_dev\n", getprogname(), pi, pd);
	} else {
		printf("%s: fd[%s] %d has st_dev = %ju\n", getprogname(), pi, pd, (uintmax_t) sb.st_dev);
	}
	if (sb.st_ino == 0) {
		printf("%s: fd[%s] %d has no st_ino\n", getprogname(), pi, pd);
	} else {
		printf("%s: fd[%s] %d has st_ino = %ju\n", getprogname(), pi, pd, (uintmax_t) sb.st_ino);
	}
	if (isapipe(pd)) {
		printf("%s: fd[%s] %d is a pipe\n", getprogname(), pi, pd);
	} else {
		printf("%s: WARNING: fd[%s] %d did not act like a pipe with lseek()!!!!\n", getprogname(), pi, pd);
		if (lseek(pd, (off_t) 0, SEEK_CUR) < 0) {
			perror("lseek(pd)");
		}
	}

	return;
}

void
test_pipe_fds(int pds[2])
{
	test_a_pipe_fd("0", pds[0]);		/* read end */
	putchar('\n');
	test_a_pipe_fd("1", pds[1]);		/* write end */
	putchar('\n');

	return;
}

int
main(int argc,
     char *argv[])
{
	int pd[2];
	pid_t child;
	char buf[PIPE_BUF];
	ssize_t n;
	int errorPipe[2];
	int fdflags;

	setprogname(argv[0]);

	(void) argc;

	test_a_pipe_fd("stdin", STDIN_FILENO);
	putchar('\n');

	pd[0] = socket(PF_LOCAL, SOCK_DGRAM, 0);
	if (pd[0] < 0) {
		perror("socket(PF_LOCAL, SOCK_RAW, 0)");
		exit(EXIT_FAILURE);
	}
	test_a_pipe_fd("socket(PF_LOCAL)", pd[0]);
	putchar('\n');
	close(pd[0]);

	pd[0] = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (pd[0] < 0) {
		perror("socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)");
		exit(EXIT_FAILURE);
	}
	test_a_pipe_fd("socket(PF_INET)", pd[0]);
	putchar('\n');
	close(pd[0]);

	if (mkfifo("tpipe.fifo", 0600) < 0) {
		perror("mkfifo()");
		exit(EXIT_FAILURE);
	}
	pd[0] = open("tpipe.fifo", O_RDONLY | O_NONBLOCK);
	if (pd[0] < 0) {
		perror("open(tpipe.fifo)");
		exit(EXIT_FAILURE);
	}
	test_a_pipe_fd("mkfifo(tpipe.fifo)", pd[0]);
	putchar('\n');
	close(pd[0]);
	unlink("tpipe.fifo");

	if (pipe(pd) < 0) {
		perror("pipe()");
		exit(EXIT_FAILURE);
	}

	test_pipe_fds(pd);

	if ((child = fork()) == -1) {
		perror("fork");
	} else if (child != 0) {

		/*
		 * This is the parent.
		 */
		printf("parent after fork...\n");

		if (write(pd[1], DATA_PARENT, sizeof(DATA_PARENT)) != sizeof(DATA_PARENT)) {
			perror("parent writing message to write end");
		}
		if ((n = read(pd[0], buf, sizeof(buf))) < 0) {
			perror("parent reading message from read end, pipes are uni-directional");
		} else {
			printf("parent read %ld bytes --> '%s'\n", (long) n, buf);
		}
		if (write(pd[0], DATA_PARENT, sizeof(DATA_PARENT)) != sizeof(DATA_PARENT)) {
			perror("parent writing a message to read end, pipes are uni-directional");
		}
		if ((n = read(pd[1], buf, sizeof(buf))) < 0) {
			perror("parent reading message from write end, pipes are uni-directional");
		} else {
			printf("parent read %ld bytes --> '%s'\n", (long) n, buf);
		}
		if (close(pd[0]) == -1) {
			perror("parent closing first descriptor");
		}
		if (close(pd[1]) == -1) {
			perror("parent closing second descriptor");
		}

		wait_child(child, "child");
	} else {
		/*
		 * This is the child.
		 */
		printf("child after fork...\n");

		if (write(pd[1], DATA_CHILD, sizeof(DATA_CHILD)) != sizeof(DATA_CHILD)) {
			perror("child writing a message to write end");
		}
		if ((n = read(pd[0], buf, sizeof(buf))) < 0) {
			perror("child reading message from read end");
		} else {
			printf("child read %ld bytes --> '%s'\n", (long) n, buf);
		}
		if (write(pd[0], DATA_CHILD, sizeof(DATA_CHILD)) != sizeof(DATA_CHILD)) {
			perror("child writing a message to read end, pipes are uni-directional");
		}
		if ((n = read(pd[1], buf, sizeof(buf))) < 0) {
			perror("child reading message from write end, pipes are uni-directional");
		} else {
			printf("child read %ld bytes --> '%s'\n", (long) n, buf);
		}
		if (close(pd[0]) == -1) {
			perror("child closing first descriptor");
		}
		if (close(pd[1]) == -1) {
			perror("child closing second descriptor");
		}
		_exit(EXIT_SUCCESS);
	}

	printf("parent after first child...\n");

	/*
	 * now we show how a child can safely signal exec*() errors back to the
	 * parent process, thus avoiding having to do logging or other more
	 * complex error handling in the child process
	 *
	 * This could also be used to "pass control safely" to a child process,
	 * e.g. to make sure the child has reached some critical point (such as
	 * acquiring a lock, opening a file. etc.) before the parent exits.
	 */

	(void) signal(SIGPIPE, SIG_IGN);

	if (pipe(errorPipe) < 0) {
		perror("pipe()");
		exit(EXIT_FAILURE);
	}

	fdflags = fcntl(errorPipe[0], F_GETFD, 0);
	if (fdflags == -1) {
		perror("fcntl(errorPipe[0], F_GETFD)");
		exit(EXIT_FAILURE);
	}
	fdflags |= FD_CLOEXEC;
	if (fcntl(errorPipe[0], F_SETFD, fdflags) == -1) {
		perror("fcntl(errorPipe[0], F_SETFD)");
		exit(EXIT_FAILURE);
	}

	fdflags = fcntl(errorPipe[1], F_GETFD, 0);
	if (fdflags == -1) {
		perror("fcntl(errorPipe[1], F_GETFD)");
		exit(EXIT_FAILURE);
	}
	fdflags |= FD_CLOEXEC;
	if (fcntl(errorPipe[1], F_SETFD, fdflags) == -1) {
		perror("fcntl(errorPipe[1], F_SETFD)");
		exit(EXIT_FAILURE);
	}

	if ((child = fork()) == -1) {
		perror("fork");
	} else if (child != 0) {
		ssize_t nr;
		char errormsg[BUFSIZ] = { '\0' };

		printf("parent after fork2...\n");

		close(errorPipe[1]);
		nr = read(errorPipe[0], errormsg, (size_t) BUFSIZ);
		printf("%s: read %u bytes: %s\n", getprogname(), (unsigned int) nr, errormsg);
		close(errorPipe[0]);
		/* since this child is expected to complete first... */
		wait_child(child, "child2");
	} else {
		struct timeval rightnow;
		char *error_message = "exec failed";

		printf("child2 after fork2...\n");

		gettimeofday(&rightnow, NULL);
		srandom((srandom_seed_t) (rightnow.tv_sec + rightnow.tv_usec));

		/* try an exec()...., sometimes pretending it failed */
		close(errorPipe[0]);
		if (random() & 01) {
			printf("child2 trying exec...\n");
			execl(PATH_TRUE, "true", (char *) NULL);
			/*
			 * a successful exec will close the pipe, causing the
			 * parent's read() call to return 0 immediately
			 */
		} else {
			printf("child2 FAILING exec...\n");
		}
		write(errorPipe[1], error_message, strlen(error_message) + 1);
		close(errorPipe[1]);
		_exit(EXIT_FAILURE);
	}
	printf("parent after second child...\n");

	exit(EXIT_SUCCESS);
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
