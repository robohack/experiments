#include <stdio.h>
#include <stdlib.h>

int
main(void)
{
    int n;
#if 0
    unsigned long long int count = 0;
#endif

    while ((n = fgetc(stdin)) >= 0) {
	    printf("%02x ", n);
#if 0
	    count++;
	    if (count % 32 == 0) {
                putchar('\n');
	    }
#endif
    }
    putchar('\n');

    exit(0);
}
