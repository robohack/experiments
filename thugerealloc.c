#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

int main(void);

/* this is a discovery of why a realloc() of sz bytes didn't error out */

int
main()
{
	size_t len, nhlen, sz;

	len = 14;


	nhlen = 4294967294U;
	printf("nhlen=%llu\n", (unsigned long long) nhlen);

	sz = len + nhlen + 2;

	printf("sz=%llu\n", (unsigned long long) sz);


	nhlen = (1 << (sizeof(size_t) * CHAR_BIT)) - 2;
	printf("nhlen=%llu\n", (unsigned long long) nhlen);

	sz = len + nhlen + 2;

	printf("sz=%llu\n", (unsigned long long) sz);


	nhlen = 0 - 2;
	printf("nhlen=%llu\n", (unsigned long long) nhlen);

	sz = len + nhlen + 2;

	printf("sz=%llu\n", (unsigned long long) sz);


	nhlen = -2;
	printf("nhlen=%llu\n", (unsigned long long) nhlen);

	sz = len + nhlen + 2;

	printf("sz=%llu\n", (unsigned long long) sz);


	nhlen = ~0 - 1;			/* ~0 == -1 */
	printf("nhlen=%llu\n", (unsigned long long) nhlen);

	sz = len + nhlen + 2;

	printf("sz=%llu\n", (unsigned long long) sz);


	exit(0);
}
