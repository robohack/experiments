#include <unistd.h>
#include <stdio.h>

int
main(argc, argv)
	int	argc;
	char	*argv[];
{
	int	i;

	for (i = 1; i < argc; i++) {
		if (truncate(argv[i], (off_t) 0) != 0)
			perror(argv[i]);
	}
	exit(0);
}
