/*
 * $ cc -o test test.c
 * $ ./test /tmp/foo.sock
 * $ ./test /tmp/foo.sock
 * test: bind(): Address already in use
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int, char **);

int
main(int argc, char **argv)
{
	struct sockaddr_un	sun;
	int			s, opt;

	if (argc != 2)
		errx(EXIT_FAILURE, "Usage: %s <path>", argv[0]);

	if ((s = socket(PF_LOCAL, SOCK_DGRAM, 0)) == -1)
		err(EXIT_FAILURE, "socket()");

	opt = 1;
	if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) == -1)
		err(EXIT_FAILURE, "setsockopt(SO_REUSEADDR)");

	sun.sun_family = AF_UNIX;
	sun.sun_len = sizeof(sun);
	(void)strlcpy(sun.sun_path, argv[1], sizeof(sun.sun_path));

	if (bind(s, (struct sockaddr *)&sun, sun.sun_len) != 0)
		err(EXIT_FAILURE, "bind()");

	/*
	 * XXX note an unlink(argv[1]) will _immediately_ remove the socket
	 * before the close() call below!
	 */

	(void)close(s);

	exit(EXIT_SUCCESS);
	/* NOTREACHED */
}
