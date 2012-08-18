#include <sys/cdefs.h>

#include <stdint.h>

#ifdef USE_VOLATILE
volatile
#endif
#ifdef USE_INT
int
#else
long long	x;
#endif

void catch_signal(int);

void
catch_signal(int sig __unused)
{
	x++;
}
