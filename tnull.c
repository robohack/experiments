#include <stdio.h>
#include <stdlib.h>

/*
 * Warning: compiling with gcc -O2 will eliminate the NULL deref at the end
 */

main()
{
	int *p;
	int foo;

	p = (int *) 0;
	p = NULL;
	foo = *(p);
}
