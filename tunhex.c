#include <stdio.h>
#include <stdlib.h>

int
main(void)
{
	int n;

	while (fscanf(stdin, "%x", &n) != EOF) {
		fputc(n, stdout);
	}

	exit(0);
}
