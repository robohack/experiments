#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

main()
{
	time_t now = time((time_t *) NULL);
	char *p;

	p = ctime(&now);
	p = strsep(&p, "\n");
	printf(">>>>%s<<<<\n", p);

	exit(0);
}
