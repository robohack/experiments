#include <sys/cdefs.h>

#include <stdint.h>

#define DO_DEFINE
#include "tatomic.h"

void
catch_signal(int sig __unused)
{
	x++;
}
