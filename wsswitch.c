#include <sys/types.h>
#include <dev/wscons/wsdisplay_usl_io.h>
#include <sys/ioctl.h>

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

int
main(int argc, char **argv) 
{
	int fd, screen;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s [1-8]\n", argv[0]);
		exit(1);
	}
	screen = atoi(argv[1]);
	if (screen < 1 || screen > 8) {
		fprintf(stderr, "Screen number %d invalid\n", screen);
		exit(1);
	}
	fd = open("/dev/ttyEcfg", O_RDWR, 0);
	if (fd == -1) {
		perror("open(/dev/ttyEcfg)");
		exit(1);
	}
	if (ioctl(fd, VT_ACTIVATE, screen) != 0) {
		fprintf(stderr, "%s: ioctl(/dev/ttyEcfg, VT_ACTIVATE, %d): %s\n", argv[0], screen, strerror(errno));
		exit(1);
	}
	close(fd);
	exit(0);
} 
