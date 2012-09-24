#include <stdio.h>
#include <stdlib.h>

int *foo(void);
int main(void);

int *
foo()
{
	int bar = 10;

	return &bar;
}

int
main()
{
	int *bad = foo();

	printf("*bad=%d\n", *bad);

	exit(0);
}
