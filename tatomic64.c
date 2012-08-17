#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int main(void);

void catch_signal(int);

#ifdef USE_VOLATILE
extern volatile
#else
extern
#endif
uint64_t	x;

int
main()
{
	(void) signal(SIGUSR1, catch_signal);

	while (1) {
		/* may evaluate to true if x is modified by ISR */
		if (!(x == x)) {
			puts("X does not equal X!");
		}

		/* may also evaluate to true if x is modified by ISR */
		if (x-x) {
			puts("X-X does not equal 0!");
		}
	}
	exit(0);
} 

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: "make tatomic64 OPTIM=-O3 CFLAGS+=-DUSE_VOLATILE LDLIBS=tatomic64-sigusr.c"
 * End:
 */ 
