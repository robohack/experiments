/*
 *	the dining philosophers
 *
 * using multiple processes and SysV semaphores
 *
 * initial implementation stolen from:
 *
 *	http://homepages.wmich.edu/~s5lavine/semaphores/#viewSource
 *
 * Question:  how does all of this relate to game theory, if at all?
 *
 * For almost any number of philosophers (>2) the following settings give
 * reasonably fair and even times:
 *
 *	tdiningphil -h 10 -g [10..90] -e 100 -t 100
 *
 * If grumpy time is larger than eating and thinking time then someone can go
 * hungry for a lot longer than everyone else.
 */

/*
 * TODO:
 *
 * - compile-time or runtime option of using POSIX semaphores(?)
 *
 * - compile-time or runtime option of using flock(2).
 *
 * - compile-time or runtime option of using lockf(2). (XPG-compat)
 *
 * - optionally allow for random eating time to solve deadlock (i.e. to solve
 *   it faster than it will be solved by external events in the OS perturbing
 *   the timing)
 *
 * - extend the lefty feature so more than one philosopher can be left-handed
 *
 * - command-line option to all the debug lines go away and maybe just print a
 *   status line showing who's thinking, eating, grumpy, etc.
 *
 * - try to figure out how to collect the timing stats from the child procs.
 *
 * - is it "hunger", as in how many times must the philosopher must eat before
 *   he's full, or is it the size of the communal bowl (they all eat until they
 *   clean up all the rice) (or his own bowl), or both or some combination?
 *   (as-is it does effectively controls the number of rounds overall)
 *
 * - another solution to guard against starvation:
 *
 * Alternately to guarantee that no philosopher starves (goes hungry for too
 * long between eating), one could keep track of the how long a hungry
 * philosopher spends thinking.  If this number exceeds some limit, the state
 * of the philosopher could change to Starving, and the decision procedure to
 * pick up forks could be augmented to require that none of the neighbours are
 * Starving.
 *
 * A philosopher that cannot pick up two chopsticks at once because a neighbour
 * is starving, is effectively waiting for the neighbour's neighbour to finish
 * eating.  This additional dependency reduces concurrency.  Raising the
 * threshold for transition to the Starving state (making the philosophers wait
 * longer before saying they are starving) reduces this effect again.
 *
 * Use message queues to ask neighbour if he's starving?  Or use shared memory?
 * (imagine a publicly visible stop watch, like a chess timer, for each
 * philosopher which records the amount of time he has been thinking since he
 * last ate)
 *
 * Effectively the same(?) is to never eat more than two more times since your
 * neighbours have eaten.
 *
 *
 * - implement Chandy / Misra solution using message queues as runtime option:
 *
 * In 1984, K. Mani Chandy and J. Misra proposed a different solution to the
 * dining philosophers problem to allow for arbitrary agents (numbered P1, ...,
 * Pn) to contend for an arbitrary number of resources, unlike Dijkstra's
 * solution.  It is also completely distributed and requires no central
 * authority after initialization.
 *
 * For this problem we can suppose the following:
 *
 * Philosophers use a pair of chopsticks to eat from a bowl (or bowls) of rice.
 * Each chopstick can either be dirty or clean.  Philosophers will only eat
 * with two clean chopsticks, but they are not allowed to clean a dirty one
 * they may have and then keep it for their own use -- they may only ever give
 * away a dirty chopstick, cleaning it in the process.
 *
 * 1. For every pair of philosophers contending for a resource (a pair of clean
 *    chopsticks), create a dirty chopstick and give it to the philosopher with
 *    the lower ID.  (can it be a random one?)
 *
 * 2. When a philosopher wants to use a set of resources (i.e. eat), he must
 *    obtain a pair of clean chopsticks from his contending neighbours.  For
 *    all such clean chopsticks he does not have, he sends a request message,
 *    then goes back to thinking.  (always msg each neighbour even if already
 *    holding a clean chopstick?)
 *
 * 3. When a thinking a philosopher with a clean chopstick receives a request
 *    message, he keeps the chopstick and denies the request.
 *
 * 4. When a thinking philosopher with a dirty chopstick receives a request the
 *    he sends the chopstick to a neighbour, cleaning it before doing so of
 *    course.
 *
 * 5. An eating philosopher ignores all requests.
 *
 * 6. After a philosopher is done eating, his chopsticks become dirty.  If
 *    another philosopher had previously requested one of them, he cleans one
 *    and sends it over to his neighbour and goes back to step 2.
 *
 * This solution allows for a large degree of concurrency, and will solve an
 * arbitrarily large problem fairly.
 *
 * It also solves the starvation problem.  The clean / dirty labels act as a
 * way of giving preference to the most "starved" processes, and a disadvantage
 * to processes that have just "eaten".  One could compare their solution to
 * one where philosophers are not allowed to eat twice in a row without letting
 * others use the forks in between.  Their solution is more flexible than that,
 * but has an element tending in that direction.
 *
 * In their analysis they derive a system of preference levels from the
 * distribution of the forks and their clean/dirty states.  They show that this
 * system may describe an acyclic graph, and if so, the operations in their
 * protocol cannot turn that graph into a cyclic one.  This guarantees that
 * deadlock cannot occur.  However, if the system is initialized to a perfectly
 * symmetric state, like all philosophers holding a clean chopstick, then the
 * graph is cyclic at the outset, and their solution cannot prevent a deadlock.
 * Initializing the system so in each pair of philosophers one, (the one with
 * lower ID?) has a dirty chopsticks ensures the graph is initially acyclic.
 */

#include <sys/cdefs.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <assert.h>
#include <err.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sysexits.h>
#include <unistd.h>

#if defined(__SVR4) && defined(__sun)
# include <siginfo.h>
#else  /* assume modern 4.4BSD-based system, or compatlib */
# define HAVE_SYS_SIG
#endif

#define	HAVE_SETPROCTITLE		/* defined */
#if defined(__APPLE__)
# undef HAVE_SETPROCTITLE
#endif

#ifndef HAVE_SETPROCTITLE
void setproctitle(const char *, ...);

void
setproctitle(
	const char *fmt		__unused,
	...)
{
	return;
}
#endif

#if defined(__APPLE__)
# define HAVE_UNION_SEMUN	1
#endif

#ifndef HAVE_UNION_SEMUN
typedef union semun {			/* semaphore union used for semctl() */
	int val;			/* value for SETVAL */
	struct semi_ds *buf;		/* buffer for IPC_STAT, IPC_SET */
	unsigned short *array;		/* array for GETALL, SETALL */
#ifdef __LINUX__
	struct seminfo *__buf;		/* buffer for IPC_INFO */
#endif
} semun_t;
#endif

extern char *optarg;
extern int optind;
extern int optopt;
extern int opterr;
extern int optreset;

#define DEFAULT_HUNGER		2	/* a light lunch */
#define DEFAULT_EATDELAY	100	/* time taken to eat */
#define DEFAULT_THINKDELAY	100	/* time will think */

/* define structure for a philosopher */
struct phil {
	char *name;		/* Philospher's name */
	unsigned int hunger;	/* units of food left to eat */
	bool lefty;		/* left-handed? */
	unsigned int leftCstick;	/* semaphore member for left-hand chopstick */
	unsigned int rightCstick;	/* semaphore member for right-hand shopstick */
};

static void usage(void);

int main(int, char *[]);

void catch_sigs(int);

pid_t seatPhil(struct phil, unsigned int);
void beHungryPhil(struct phil, unsigned int);
int collectPhil(void);

int init_sem(unsigned int);
void clear_sem(void);

bool pickChopstick_P(unsigned int, int/*bool*/);
bool releaseChopstick_V(unsigned int);

useconds_t difftval(struct timeval, struct timeval);
useconds_t microsleep(useconds_t);

/* global vars */
int sid;			/* shared semaphore ID */
useconds_t eatTime = DEFAULT_EATDELAY;
useconds_t thinkTime = DEFAULT_THINKDELAY;
useconds_t grumpyTime = 0;
bool deadly = false;
bool lefty = false;
bool greedy = false;
bool swapOrder = false;
const char *argv0 = "progname";

unsigned int philc;		/* philosopher count, also philID's */
pid_t *philPidByID;		/* PIDs, indexed by philID */

/* private vars */
static char const *def_phil_nms[] = {
	"Aristotle",
	"Kant",
	"Spinoza",
	"Marx",
	"Russell"
};

const unsigned int num_phil_nms = sizeof(def_phil_nms) / sizeof(def_phil_nms[0]);


__dead static void
usage()
{
	fprintf(stderr, "Usage: %s [-d] [-G|-s] [-g grumpytm] [-h hungerlvl] [-e eattm] [-t thinktm] phils\n", argv0);
	exit(EX_USAGE);
}


int
main(
	int argc,
	char *argv[])
{
	int ch;
	int ti;
	unsigned int hunger = DEFAULT_HUNGER;	/* philosopher hunger level */
	unsigned int i;			/* iterative variable used in loops */
	struct phil phils;		/* philosopher structure */
	pid_t pid;			/* children process ids of philosophers */

	argv0 = (argv0 = strrchr(argv[0], '/')) ? argv0 + 1 : argv[0];

	if (sizeof(useconds_t) < sizeof(unsigned long)) {
		warnx("timekeeping may overflow as sizeof(useconds_t) < sizeof(unsigned long)");
	}

	optind = 1;		/* Start options parsing */
	opterr = 0;		/* I'll print my own errors! */
	while ((ch = getopt(argc, argv, ":de:Gg:h:lst:")) != -1) {
		switch (ch) {
		case 'd':
			/*
			 * block waiting until the left chopstick available
			 *
			 * XXX does this make deadlock more or less likely?
			 */
			deadly = true;
			break;
		case 'e':
			ti = atoi(optarg); /* XXX strtoul() */
			if (ti <= 0) {
				errx(EX_USAGE, "philosophers must eat for at least one second!");
			}
			eatTime = (useconds_t) ti;
			break;
		case 'G':
			/*
			 * never put down the first chopstick picked up!
			 *
			 * prove we can deadlock!
			 */
			greedy = true;
			break;
		case 'g':
			ti = atoi(optarg); /* XXX strtoul() */
			if (ti < 0) {
				errx(EX_USAGE, "invalid grumpy time");
			}
			grumpyTime = (useconds_t) ti;
			break;
		case 'h':
			ti = atoi(optarg);
			if (ti <= 0) {
				errx(EX_USAGE, "philosophers who are not hungry don't eat!");
			}
			hunger = (unsigned int) ti;
			break;
		case 'l':
			lefty = true;
			break;
		case 's':
			swapOrder = true;
			break;
		case 't':
			ti = atoi(optarg); /* XXX strtoul() */
			if (ti < 0) {
				errx(EX_USAGE, "invalid think time");
			}
			thinkTime = (useconds_t) ti;
			break;
		case '?':
			warnx("unknown option -- '%c'", optopt);
			usage();
		case ':':
			warnx("missing parameter for -%c", optopt);
			usage();
		default:
			warnx("programming error, unhandled flag: %c", ch);
			abort();
		}
	}
	argc -= optind;
	argv += optind;

	/* get the (optional) philosopher count */
	if (argc == 1) {
		ti = atoi(argv[0]);
		if (ti < 2) {
			errx(EX_USAGE, "there must be at least 2 philosophers");
		}
		philc = (unsigned int) ti;
	} else if (argc == 0) {
		philc = num_phil_nms;
		printf("Setting a table for %d....\n", philc);
	} else {
		usage();
	}

	if (greedy && swapOrder) {
		warnx("combining '-g' and '-s' is nonsensical");
		usage();
	}

	if ((philPidByID = calloc((size_t) philc, sizeof(pid_t))) == NULL) {
		err(EXIT_FAILURE, "failed to allocate PID array");
	}
	if ((sid = init_sem(philc)) < 0) {
		errx(EXIT_FAILURE, "failed to initialize semaphores");
	}

	/*
	 * XXX there's a tiny race here where we could be killed without
	 * cleaning up the semaphore or killing the kids....
	 */
	(void) signal(SIGINT, catch_sigs);
	(void) signal(SIGTERM, catch_sigs);

#if 0
	/*
	 * in theory we don't need to do this as the shell should have
	 */
	(void) setpgrp(0, getpid());
#endif

	/*
	 * create the philosophers (as child processes)
	 *
	 * A chopstick is laid down between each philsopher, and each is given
	 * a "pointer" to the left-hand one, and a "pointer" to the right-hand
	 * one.
	 *
	 * Their identities (philID) are simply their ordinal numbers from zero
	 * through to one less than the number of philosophers.
	 *
	 * Their PIDs are stored in an array indexed by their ID.
	 */
	for (i = 0; i < philc; ++i) {
		if (i == 0) {
			/* zero is such an odd number, it must be the lefty! */
			phils.lefty = true;
		} else {
			phils.lefty = false;
		}
		phils.hunger = hunger;

		if (i < num_phil_nms) {
			phils.name = strdup(def_phil_nms[i]);
		} else {
			if (asprintf(&phils.name, "Philosopher-%d", i) == -1) {
				err(EXIT_FAILURE, "failed to name additional philosopher %d", i);
			}
		}

		/* the left-hand chopstick */
		phils.leftCstick = i;
		/* the right-hand chopstick */
		phils.rightCstick = (i == 0) ? philc - 1 : i - 1;

		if ((pid = seatPhil(phils, i)) < 0) {
			errx(EXIT_FAILURE, "failed to seat philosopher %d!", i);
		}
		/* wait for process to send itself SIGSTOP */
		waitpid(pid, NULL, WUNTRACED);
		philPidByID[i] = pid;
	}

	/*
	 * NOTE:  must do this after the philosophers are running, else one
	 * finishing early will delete the semaphore and cause problems
	 */
	if (atexit(clear_sem) < 0) {
		err(EXIT_FAILURE, "failed to register dishwasher function");
	}

	/*
	 * send the "begin" signal to all philosophers
	 *
	 * If all five philosophers appear in the dining room at exactly the
	 * same time and each picks up the left left chopstick at the same time
	 * the philosophers will wait one unit of thinking time until they all
	 * all try picking them up again.
	 *
	 * We can't tell if the kernel will always wake and schedule them in
	 * the same order or not.
	 *
	 * Note: it's OK to SIGCONT our own process.  :-)
	 */
	kill(0, SIGCONT);

	/* collect the cheques as the philosophers finish eating */
	for (i = 0; i < philc; ++i) {
		unsigned int j;

		if ((pid = collectPhil()) < 0) {
			continue;
		}
		for (j = 0; j < philc; ++j) {
			if (philPidByID[j] == pid) {
				philPidByID[j] = 0;
				break;
			}
		}
		if (j == philc) {
			warnx("unknown philosopher PID %u!", (unsigned int) pid);
		}
		fprintf(stderr, "DEBUG: Philosopher %d left the table.\n", j);
	}

	printf("All philsopher's are done eating!\n");

	exit(EXIT_SUCCESS);
}

/**
 * pid_t seatPhil()
 *
 * forks a new process simulating a dining philosopher
 *
 * @param philState - structure representing philosopher
 * @param philID - philosopher id number
 *
 * @return child process ID on success, -1 on error
 */
pid_t
seatPhil(
	struct phil philState,
	unsigned int philID)
{
	pid_t pid;			/* process id */

	if ((pid = fork()) < 0) {
		warn("fork() failed");
		return -1;
	} else if (pid > 0) {		/* parent process */
		return pid;
	}

	/* child process -- be a hungry philosopher! */

	signal(SIGINT, SIG_DFL);
	signal(SIGTERM, SIG_DFL);

	setproctitle("Philosopher ID %d [%s]", philID, philState.name);

	/* stop the process to allow for creation of other philosophers */
	fprintf(stderr, "%s pausing...\n", philState.name);
	kill(getpid(), SIGSTOP);

	fprintf(stderr, "%s continuing...\n", philState.name);

	beHungryPhil(philState, philID);

	/* leave the table */
	exit(0);
}

/**
 * void beHungryPhil()
 *
 * be a hungry self-centred uncommunicative philosopher
 *
 * @param philState - structure representing philosopher's state
 * @param philID - philosopher id number
 *
 * @return (void) when done eating
 *
 * Try to eat.  Think when not eating.
 */
void
beHungryPhil(
	struct phil philState,
	unsigned int philID)
{
	int i;
	bool failed = false;
	useconds_t waitTime = 0;
	useconds_t totalThinkTm = 0;
	useconds_t nextThinkTime = thinkTime;
	useconds_t TotalTime;		/* total run time */
	useconds_t tttm;		/* temporary thinking time */
	useconds_t ettm = 0;		/* total eat time */
	struct timeval ttms;
	struct timeval ttmf;
	struct timeval dttms;
	struct timeval dttmf;

	gettimeofday(&ttms, (void *) NULL);

	i = (int) philState.hunger;

	while (i > 0) {
		static unsigned int firstCstick;
		static const char *firstCstickName = NULL;
		static unsigned int secondCstick;
		static const char *secondCstickName = NULL;

		if (!swapOrder || !firstCstickName) {
			if (philState.lefty) {
				firstCstick = philState.leftCstick;
				firstCstickName = "left";
				secondCstick = philState.rightCstick;
				secondCstickName = "right";
			} else {
				firstCstick = philState.rightCstick;
				firstCstickName = "right";
				secondCstick = philState.leftCstick;
				secondCstickName = "left";
			}
		} else if (failed) {
			unsigned int tCstick;
			const char *tCstickName;

			/* swap order */
			tCstick = firstCstick;
			tCstickName = firstCstickName;
			firstCstick = secondCstick;
			firstCstickName = secondCstickName;
			secondCstick = tCstick;
			secondCstickName = tCstickName;
			fprintf(stderr, "DEBUG: Philosopher %d has SWAPPED pickup order\n",
				philID);
		}

		if (grumpyTime > 0 && waitTime > grumpyTime) {
			nextThinkTime = 0;
		}

		if (waitTime > 0)
			fprintf(stderr, "DEBUG: Philosopher %d has %d units of food left to eat, been waiting %luus\n",
				philID, i, (u_long) waitTime);
		else
			fprintf(stderr, "DEBUG: Philosopher %d has %d units of food left to eat\n",
				philID, i);

		/* pick up first chopstick */
		gettimeofday(&dttms, (void *) NULL);
		if (!pickChopstick_P(firstCstick, deadly)) {
			fprintf(stderr, "DEBUG: Philosopher %d failed to pick up %s chopstick, %s\n",
				philID, firstCstickName,
				nextThinkTime ? "thinking...."
					      : "REAL GRUMPY!");
			tttm = microsleep(nextThinkTime);
			totalThinkTm += tttm;
			waitTime += tttm;
			failed = true;
			continue;	/* retry eating */
		} if (deadly) {
			unsigned long bttm; /* total time blocked */

			/*
			 * "deadly" means we blocked waiting for the first
			 * chopstick to come available, so we have to account
			 * for the amount of time blocked as thinking (and
			 * waiting hungry) time
			 */
			gettimeofday(&dttmf, (void *) NULL);
			bttm = difftval(dttms, dttmf);
			/* n.b.:  assume time always increases */
			totalThinkTm += (useconds_t) bttm;
			waitTime += (useconds_t) bttm;
		}

		fprintf(stderr, "DEBUG: Philosopher %d has his %s-hand chopstick\n",
			philID, firstCstickName);

		/* pick up second chopstick */
		if (!pickChopstick_P(secondCstick, false)) {
			fprintf(stderr, "DEBUG: Philosopher %d failed to pick up %s chopstick\n",
				philID, secondCstickName);
			if (!greedy) {
				if (!releaseChopstick_V(firstCstick)) {
					fprintf(stderr, "ERROR: Philosopher %d failed to put down %s chopstick\n",
						philID, firstCstickName);
				}
				fprintf(stderr, "DEBUG: Philosopher %d put down %s chopstick, %s\n",
					philID, firstCstickName,
					nextThinkTime ? (swapOrder ? "SWAPPING"
								   : "thinking....")
						      : "GRUMPY!");
			} else if (!swapOrder && nextThinkTime > 0) {
				/* we never swap order, but we may delay */
				tttm = microsleep(nextThinkTime); /* Think.... */
				totalThinkTm += tttm;
				waitTime += tttm;
			}
			failed = true;
			continue;	/* retry eating */
		}
		failed = false;
		waitTime = 0;
		nextThinkTime = thinkTime;

		fprintf(stderr, "DEBUG: Philosopher %d has two chopsticks and is eating%s\n",
				philID, i > 1 ? "" : " his last unit of food");

		/* eat a unit of food */
		ettm += microsleep(eatTime);
		--i;

		/* put down first chopstick */
		if (!releaseChopstick_V(firstCstick)) {
			fprintf(stderr, "ERROR: Philosopher %d failed to put down %s chopstick\n",
				philID, firstCstickName);
		}
		fprintf(stderr, "DEBUG: Philosopher %d put down %s chopstick\n",
			philID, firstCstickName);

		/* put down second chopstick */
		if (!releaseChopstick_V(secondCstick)) {
			fprintf(stderr, "ERROR: Philosopher %d failed to put down %s chopstick\n",
				philID, secondCstickName);
		}

		fprintf(stderr, "DEBUG: Philosopher %d put down %s chopstick%s\n",
			philID, secondCstickName, i > 0 ? " and is thinking...." : "");

		if (i > 0) {
			/* Think some before eating more.... */
			totalThinkTm += microsleep(thinkTime);
		}
	}
	gettimeofday(&ttmf, (void *) NULL);
	TotalTime = difftval(ttms, ttmf);

	/*
	 * 012345678901234
	 * Philosopher-NNN
	 */
	printf("%14s: done: t=%lu (at=%lu) s/r=%lu?  [e=%lu] [tt=%lu]\n",
	       philState.name, (u_long) totalThinkTm, (u_long) TotalTime-ettm,
	       (u_long) TotalTime-ettm-totalThinkTm,
	       (u_long) ettm, (u_long) TotalTime);

	return;
}

void
catch_sigs(int sig)
{
	unsigned int i;

	for (i = 0; i < philc; i++) {
		if (philPidByID[i] > 0) {
			if (kill(philPidByID[i], sig) < 0) {
				if (errno == ESRCH) {
					warnx("Philosopher ID %d [PID %ld] was already dead",
						i, (long) philPidByID[i]);
				} else {
					warn("Philosopher ID %d: kill(%ld, SIG%s)",
					     i, (long) philPidByID[i], sys_signame[sig]);
				}
			}
		}
	}
#if 0
	(void) kill(0, sig);		/* XXX for good measure */
#endif

	/*
	 * do this to be sure we whack the semaphore
	 *
	 * XXX unfortunately it's said that it's not safe to use exit(3) in a
	 * signal handler......  Hmmmm, is that so?  why?  signal(7) on NetBSD
	 * says this is because exit(3) is not "async-signal-safe".  Presumably
	 * this is because calling the atexit() handlers may be done in a
	 * non-reentrant fashion if libc is built without #define __REENTRANT.
	 *
	 * Oddly abort(3) is allowed by signal(7), and it does all the same
	 * things, things which are not protected by a mutex, as are done in
	 * the NetBSD implementation of exit(3).
	 *
	 * We could call clear_sem() and _exit() instead, but if we know all
	 * our atexit() handlers are async-signal-safe then let's just use
	 * exit() here.
	 *
	 * Besides, we don't really care -- we're not (supposed to be) setuid.
	 */
	exit(EXIT_FAILURE);
}

int
init_sem(unsigned int cnt)
{
	unsigned int i;
	int sem_id;
	semun_t sunion;

	/* create semaphore set for chopsticks (1 stick per philosopher) */
	if ((sem_id = semget(IPC_PRIVATE, (int) cnt, IPC_CREAT | 0700)) < 0) {
		warn("failed to create semaphore ID");
		return -1;
	}

	/* initialize each chopstick (1 unit available per philosopher) */
	sunion.val = 1;
	for (i = 0; i < cnt; ++i) {
		if (semctl(sem_id, (int) i, SETVAL, sunion) != 0) {
			warn("failed to initialize semaphore #%d", i);
			return -1;
		}
	}

	return sem_id;
}

void
clear_sem()
{
	if (sid < 0) {
		return;
	}
	/*
	 * XXX semctl(2) isn't listed as async-signal-safe in signal(7) [on
	 * NetBSD], but how could it not be?
	 *
	 * Neither is warn(3), but do we don't need to care -- we're not
	 * (supposed to be) setuid.
	 *
	 * exit(3) isn't either of course, but see the discussion in the
	 * comments in catch_sigs() above.
	 */
	if (semctl(sid, 0, IPC_RMID) != 0) {
		warn("failed to wash up chopsticks");
		sid = -1;		/* don't loop! */
		exit(EXIT_FAILURE);
	}
}

/**
 * int pickChopstick_P()
 *
 * Obtains a chopstick
 *
 * @param chopstick - ID number of chopstick to obtain
 * @param block - whether to wait or return immediately
 *
 * @return true when chopstick is obtained, false if in use
 *
 * "P" stands for "prolagen", a made up word derived from the Dutch phrase
 * "proberen te verlagen" which means "try to decrease".
 */
bool
pickChopstick_P(
	unsigned int chopstick,
	int block)			/* XXX bool */
{
	struct sembuf sbuf;		/* semaphore buffer for semop() */

	sbuf.sem_op = -1;		/* decrease by 1 to obtain resource */
	sbuf.sem_num = (unsigned short) chopstick;
	sbuf.sem_flg = SEM_UNDO;	/* avoid deadlock afer abort() */
	if (!block)
		sbuf.sem_flg = IPC_NOWAIT;

	if (semop(sid, &sbuf, (size_t) 1) != 0) {
		if (errno != EAGAIN) {
			assert(!block);
			err(EXIT_FAILURE, "failed to pick up chopstick #%d", chopstick);
		}
		return false;
	}

	return true;
}

/**
 * int releaseChopstick_V()
 *
 * Releases a chopstick
 *
 * @param chopstick - ID number of chopstick to release
 *
 * @return true on success, false on error
 *
 * "V" stands for "verhogen", Dutch for "increase".
 */
bool
releaseChopstick_V(unsigned int chopstick)
{
	struct sembuf sbuf;		/* semaphore buffer for semop() */

	sbuf.sem_op = +1;		/* increase by 1 to release resource */
	sbuf.sem_num = (unsigned short) chopstick;
	sbuf.sem_flg = IPC_NOWAIT;	/* XXX don't need SEM_UNDO? */

	if (semop(sid, &sbuf, (size_t) 1) != 0) {
		if (errno != EAGAIN) {
			err(EXIT_FAILURE, "failed to release chopstick #%d", chopstick);
		}
		return false;		/* impossible? */
	}
	return true;
}

int
collectPhil()
{
	pid_t pid;
	int status;

	if ((pid = wait(&status)) == -1) {
		warn("%s(): wait() failed", __func__);
		return -1;
	}
	if (!WIFEXITED(status)) {
		if (WIFSIGNALED(status)) {
			fprintf(stderr,
				"PID %u failed: killed by SIG%s: %s\n",
				(unsigned int) pid,
				sys_signame[WTERMSIG(status)],
				sys_siglist[WTERMSIG(status)]);
		} else if (WIFSTOPPED(status)) {
			fprintf(stderr,
				"PID %u failed: stopped by SIG%s: %s\n",
				(unsigned int) pid,
				sys_signame[WSTOPSIG(status)],
				sys_siglist[WSTOPSIG(status)]);
		} else { /* impossible? */
			fprintf(stderr, "PID %u failed: not-exited, not-stopped, status %d\n",
				(unsigned int) pid,
				status);
		}
	} else if (WEXITSTATUS(status) != 0) {
		fprintf(stderr, "PID %u terminated with %d (non-zero) status\n",
			(unsigned int) pid,
			WEXITSTATUS(status));
	}

	return pid;
}

useconds_t
difftval(struct timeval tstart, struct timeval tend)
{
	useconds_t diff;

	tend.tv_sec -= tstart.tv_sec;
	tend.tv_usec -= tstart.tv_usec;
	if (tend.tv_usec < 0) {
		tend.tv_sec--;
		tend.tv_usec += 1000000;
	}
	if (tend.tv_usec >= 1000000) {
		tend.tv_sec++;
		tend.tv_usec -= 1000000;
	}

	diff = (useconds_t) ((tend.tv_sec * 1000000) + tend.tv_usec);

	return diff;
}

/*
 * sleep for some number of microseconds
 *
 * return the actual number of microseconds spent unconsious
 */
useconds_t
microsleep(useconds_t ms)
{
	struct timeval tms;
	struct timeval tmf;

	gettimeofday(&tms, (void *) NULL);
	/*
	 * n.b.:  POSIX-1003.1-2004 deprecated usleep(), and POSIX-1003.1-2008
	 * removed it entirely
	 */
	/* BSDs require usleep()'s param to be less than one million */
	if (ms >= 1000000) {
		sleep(ms / 1000000);
		usleep(ms % 1000000);
	} else {
		usleep(ms);
	}
	gettimeofday(&tmf, (void *) NULL);

	return difftval(tms, tmf);
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm -f " fn "; " (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
