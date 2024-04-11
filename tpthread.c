#include <pthread.h>
#include <err.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define MANAGE_PTHREADS			/* defined -- call pthread_join() */

enum {
	PRINT_REPEAT = 10,
	NUM_THREADS = 4,
};

int global_exit = EXIT_SUCCESS;		/* xxx need a mutex for this */

static void
some_cleanup(void)
{
	printf("%s: All Done.\n", __func__);

#ifndef MANAGE_PTHREADS
	printf("%s: calling _exit(%d).\n", __func__, global_exit);

	/*
	 * xxx this is a very bad hack as we cannot know if we're the last
	 * atexit() handler to be called, but it would allow for main to call
	 * pthread_exit() and for any other thread to trigger an EXIT_FAILURE.
	 */
	_exit(global_exit);
#endif
}

struct my_thread_arg {
	unsigned int num;
	pthread_t id;
	int exit_code;
};

static void *
PrintHello(void *arg)
{
	struct my_thread_arg *param;
	unsigned int i;

	param = arg;

	for (i = 0; i < PRINT_REPEAT; i++) {
		/*
		 * On systems with a very fine-grained thread scheduler you
		 * should only see one line at a time from any given thread.
		 * (e.g. macOS)
		 */
		printf("Hello World!  It's me, thread # %u [id:%p]!\n",
		       param->num, param->id);
#ifndef MAAGE_PTHREADS
		/* if there were an error to report we could set global_exit */
#else
		/* if there were an error to report we could set it in param->exit_code */
#endif
	};

#ifdef MANAGE_PTHREADS
	/* main() will be calling pthread_join() to acquire 'arg' */
	pthread_exit(arg);
	/* NOTREACHED */
#else
	free(arg);

	return NULL;
#endif
}

int main(void);

int
main()
{
	pthread_t threads[NUM_THREADS];
	int rc;
	unsigned int t;

	if (atexit(some_cleanup) < 0) {
		warn("failed to register dishwasher function");
	}

	printf("PrintHello() is at %p\n", (void *) PrintHello);

	for (t = 0; t < NUM_THREADS; t++) {
		struct my_thread_arg *tp;

		tp = malloc(sizeof(*tp));
		memset(tp, 0, sizeof(*tp));
		tp->num = t;
		rc = pthread_create(&threads[t], NULL, PrintHello, tp);
		/*
		 * xxx we should have a mutex in tp that would be un-locked only
		 * after this assignment, and then have PrintHello() block on
		 * that mutex before starting any work....
		 */
		tp->id = threads[t];
		if (rc != 0) {
			err(EXIT_FAILURE, "ERROR; pthread_create() failed");
		}
	}

#ifndef MANAGE_PTHREADS
	pthread_exit(NULL);		/* waits until all child threads exit */
	/* xxx no proper exit code for main()??? */
	/*
	 * XXX there is no call to functions registered by atexit().
	 *
	 * POSIX says:
	 *
	 *	The process exits with an exit status of 0 after the last thread
	 *	has been terminated. The behaviour is as if the implementation
	 *	called exit() with a zero argument at thread termination time.
	 *
	 * http://pubs.opengroup.org/onlinepubs/007908799/xsh/pthread_exit.html
	 *
	 * To me this implies that exit(0) should be called, thus functions
	 * registered by atexit() should also be called!
	 *
	 * XXX however it doesn't seem many (any?) platforms do that....
	 */
#else
	/* explicitly wait for each child thread to finish */
	do {
		void *exval = NULL;

		(void) pthread_join(threads[--t], &exval);
		if (exval) {
			struct my_thread_arg *tp;

			tp = exval;
			global_exit |= tp->exit_code;
			free(exval);
			exval = NULL;
		}
	} while (t > 0);

	exit(global_exit);
#endif
	/* NOTREACHED */
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm -f " fn "; " (default-value 'compile-command) " LDLIBS=-lpthread " fn " && ./" fn))
 * End:
 */
