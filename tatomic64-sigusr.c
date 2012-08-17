#include <sys/cdefs.h>

#include <stdint.h>

#ifdef USE_VOLATILE
volatile
#endif
uint64_t	x;

void catch_signal(int);

void
catch_signal(int sig __unused)
{
	x++;
}
