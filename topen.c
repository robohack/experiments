#include <fcntl.h>

main(argc, argv)
	int argc;
	char *argv[];
{
	if (open("foo.open", O_WRONLY | O_APPEND, 0644) < 0)
		perror("foo.open");

	exit(0);
}
