#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>

static int make_gcc_barf_on_alpha __P((void));

static int
make_gcc_barf_on_alpha()
{
	unsigned long inet = 0;		/* an IPv4 address */

#if 1
	printf("parse_address(): inet addr given: [%s]\n",
	       inet_ntoa(*((struct in_addr *) &inet)));
#else
	printf("parse_address(): inet addr given: [%s]\n", target,
	       inet_ntoa(inet_makeaddr(inet, (unsigned long) 0)));
#endif
}
