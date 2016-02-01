/*
 * Adapted from Russel Cox: http://research.swtch.com/sigtest.c
 *
 * Test whether profiling signals are sent to the thread
 * whose execution triggered the signal.
 */
#include <sys/time.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static void handler(int);
static void* looper(void*);

enum {
	Nthread = 4,
};

static pthread_t pmain, ploop[Nthread];
static int maincount, loopcount[Nthread];

int
main(void)
{
	int i;
	pthread_cond_t cond;
	pthread_mutex_t mu;

	pmain = pthread_self();
	for(i=0; i<Nthread; i++)
		pthread_create(&ploop[i], 0, looper, 0);

	pthread_mutex_init(&mu, 0);
	pthread_mutex_lock(&mu);

	pthread_cond_init(&cond, 0);
	for (;;)
		pthread_cond_wait(&cond, &mu);

	/* NOTREACHED */
	exit(0);
}

static void
handler(int sig)
{
	static volatile int nsig;
	pthread_t p;
	int i, n;

	p = pthread_self();
	if (p == pmain) {
		if(nsig < 5)
			printf("signal on sleeping main thread\n");
		maincount++;
	} else {
	    for (i = 0; i < Nthread; i++) {
		if (p == ploop[i]) {
		    if (nsig < 5)
			printf("signal %d on cpu-chewing looper thread %d\n", sig, i);
		    loopcount[i]++;
		    break;
		}
	    }
	    if (i == 4)
		printf("signal %d on unknown thread %p\n", sig, (void*)p);
	}
	n = nsig;
	n++;
	nsig = n;
	if (n == 5)
		printf("... still running but not printing signals ...\n");
	if (n == 500) {
		printf("%d signals received.\n", nsig);
		printf("%d on main\n", maincount);
		for (i = 0; i < Nthread; i++)
			printf("%d on looper %d\n", loopcount[i], i);
		exit(0);
	}
}

static void*
looper(void *v)
{
	struct itimerval it;
	struct sigaction sa;

	memset(&sa, 0, sizeof sa);
	sa.sa_handler = handler;
	sa.sa_flags = SA_RESTART;
	memset(&sa.sa_mask, 0xff, sizeof sa.sa_mask);
	sigaction(SIGPROF, &sa, 0);

	memset(&it, 0, sizeof it);
	it.it_interval.tv_usec = 10000;
	it.it_value = it.it_interval;
	setitimer(ITIMER_PROF, &it, 0);

	(void) v;
	for (;;);
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat (default-value 'compile-command) " LDLIBS=-lpthread " fn " && ./" fn))
 * End:
 */
