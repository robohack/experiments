#include <stdint.h>
#include <stdio.h>


int main(void);

int
main()
{
	unsigned short x = (USHORT_MAX + 1) - 3; // Or just -3

	printf("x = %u\n", x);
	x*=x;
	printf("x = %u\n", x);

	return 0;
}
