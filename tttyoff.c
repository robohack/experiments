#include <stdio.h>

#include <sys/tty.h>

main()
{
	struct tty t;

	printf("%lx\n", (long) &(t.t_outq) - (long) &t);
}
