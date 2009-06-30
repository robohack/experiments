#include <limits.h>
#include <stdio.h>

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

	r = idiv64(x, y);

	printf("%lld\n", r);

	exit(0);
}
