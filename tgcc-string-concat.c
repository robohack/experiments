#include <sys/cdefs.h>

#if 1
/*
 * NOTE: as of netbsd-5 __COPYRIGHT()'s parameter is an as(1) string (i.e. data
 * for a .asciz statement), not a C string.
 */
__COPYRIGHT("@(#) Copyright (c) 1991, 1993\
 The Regents of the University of California.  All rights reserved.");
#else
__COPYRIGHT("@(#) Copyright (c) 1991, 1993\n"
"	The Regents of the University of California.  All rights reserved.\n");
#endif

#include <stdlib.h>

int main(int, char *[]);

int
main(int argv __unused, char *argc[] __unused)
{
	exit(0);
}
