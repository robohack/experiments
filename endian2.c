#include <stdio.h>

/*
 * from Harbison & Steele, 4th ed., pg. 164
 */

union {
	long mylong;
	char mychar[sizeof(long)];
} un;

int
main()
{
	un.mylong = 1;
	if (un.mychar[0] == 1)
		printf("Addressiong is right-to-left (little-endian).\n");
	else if (un.mychar[sizeof(long)-1] == 1)
		printf("Addressing is left-to-right (big-endian).\n");
	else
		printf("Addressing is strange (pdp11?).\n");
	exit(0);
}
