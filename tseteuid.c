#include <stdio.h>

main()
{
#if 0
	printf("getuid = %d, geteuid = %d\n", getuid(), geteuid());
	if (seteuid(getuid()) < 0)
		perror("seteuid(uid)");
	printf("getuid = %d, geteuid = %d\n", getuid(), geteuid());
	if (seteuid(1) < 0)
		perror("seteuid(1)");
	printf("getuid = %d, geteuid = %d\n", getuid(), geteuid());
	if (seteuid(0) < 0)
		perror("seteuid(0)");
#endif
	printf("getuid = %d, geteuid = %d\n", getuid(), geteuid());
	if (setuid(1) < 0)
		perror("setuid(1)");
	printf("getuid = %d, geteuid = %d\n", getuid(), geteuid());
	if (setuid(getuid()) < 0)
		perror("setuid(uid)");
	printf("getuid = %d, geteuid = %d\n", getuid(), geteuid());
	exit(0);
}
