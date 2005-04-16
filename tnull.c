#include <stdio.h>
#include <stdlib.h>

main()
{
	int *p;
	int foo;

	p = (int *) 0;
	p = NULL;
	foo = *(p);
}
