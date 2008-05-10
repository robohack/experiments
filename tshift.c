#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/param.h>

main()
{
	puts("Hello World!");
	printf("mclbytes = %u.\n", (unsigned int) MCLBYTES);
}
