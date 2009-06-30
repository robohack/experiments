#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

long long int
idiv64(long long int x,
       long long int y)
{
	return (x/y);
}

int
main(void)
{
	long long int x = LLONG_MIN;
	long long y = -1;
	long long r = 1234567890;

	printf("x = %lld\n", x);
	printf("y = %lld\n", y);

	r = idiv64(x, y);

	printf("x / y = %lld%s\n", r, r == LLONG_MIN ? " (LLONG_MIN)" : "");

	r = x / 0;
	printf("x / 0 = %lld\n", r);

	exit(0);
}
