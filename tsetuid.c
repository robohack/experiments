#include <stdio.h>

main()
{
	fprintf(stderr, "setuid(65533) = %d: ", setuid(65533));
	perror("");
	fputc('\n', stderr);
	exit(0);
}
