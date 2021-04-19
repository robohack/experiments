#include <sys/param.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int do_create(int argc, char* argv[]);
static int do_check(int argc, char* argv[]);
static off_t mkoff(char* line);
static int mkbuf(char* buf, char* line);
static void do_cmp(int fd, off_t offset, char* buf, int buflen);
static void usage(FILE* fp);

enum { MAX_READ = 64 };
static char* prog;

int
main(int argc, char* argv[])
{
	int rc = EXIT_FAILURE;
	prog = argv[0];

	if (argc < 2) {
		usage(stderr);
		exit(rc);
	}

	if (strcmp(argv[1], "create") == 0) {
		argc -= 2; argv += 2;
		rc = do_create(argc, argv);
	} else if (strcmp(argv[1], "check") == 0) {
		argc -= 2; argv += 2;
		rc = do_check(argc, argv);
	} else if (strcmp(argv[1], "help") == 0) {
		usage(stdout);
		rc = EXIT_SUCCESS;
	} else
		warnx("%s: unknown command", argv[1]);

	return rc;
}

static int
do_create(int argc, char* argv[])
{
	char* devname, *s, *end;
	int fd, n, rc = EXIT_FAILURE;
	long nsect, nread;

	if (argc != 3) {
		warnx("create: not enough args.");
		return rc;
	}

	/* nsect */
	s = argv[1];
	if (*s == '\0') {
		warnx("empty nsect value");
		return rc;
	}
	errno = 0;
	nsect = strtol(s, &end, 10);
	if (nsect == 0 && (errno == EINVAL || errno == ERANGE)) {
		warn("bad nsect value `%s'", s);
		return rc;
	}
	if (nsect <= 0 || *end != '\0') {
		warnx("bad nsect value `%s'", s);
		return rc;
	}

	/* nread */
	s = argv[2];
	if (*s == '\0') {
		warnx("empty nread value");
		return rc;
	}
	errno = 0;
	nread = strtol(s, &end, 10);
	if (nread == 0 && (errno == EINVAL || errno == ERANGE)) {
		warn("bad nread value `%s'", s);
		return rc;
	}
	if (nread <= 0 || *end != '\0') {
		warnx("bad nread value `%s'", s);
		return rc;
	}

	/* device/file name */
	devname = argv[0];
	if ((fd = open(devname, O_RDONLY)) == -1) {
		warn("%s: open failed.", devname);
		return rc;
	}

	srandom(getpid() ^ getppid());
	for (n = 0; n < nread; ) {
		char buf[DEV_BSIZE];
		int i;
		off_t offset = (random() % nsect) * DEV_BSIZE;
		long nr = random() % MAX_READ;

		if (nr == 0)
			continue;
		if (lseek(fd, offset, SEEK_SET) == -1)	
			err(rc, "%s: lseek offset `%lu' failed", devname, offset);
		if (read(fd, buf, DEV_BSIZE) != DEV_BSIZE)
			err(rc, "%s: read `%u' bytes failed", devname, DEV_BSIZE);
		printf("%lu", offset);
		for (i = 0; i < nr; i++)
			printf(" %02x", (unsigned char)buf[i]);
		printf("\n");
		n++;
	}

	close(fd);
	rc = EXIT_SUCCESS;

	return rc;
}

static int
do_check(int argc, char* argv[])
{
	char* devname;
	char* fname;
	char line[16 + MAX_READ*3 + 1];
	int fd, rc = EXIT_FAILURE;
	FILE* fp;

	if (argc != 2) {
		warnx("check: not enough args.");
		return rc;
	}

	/* device/file name */
	devname = argv[0];
	if ((fd = open(devname, O_RDONLY)) == -1) {
		warn("%s: open failed.", devname);
		return rc;
	}

	/* check-file name */
	fname = argv[1];
	if ((fp = fopen(fname, "r")) == NULL) {
		warn("%s: fopen failed.", fname);
		close(fd);
		return rc;
	}

	while (fgets(line, sizeof line, fp) != NULL) {
		char buf[MAX_READ];
		char* p;
		off_t offset;
		int n;

		if ((p = strrchr(line, '\n')) != NULL)
			*p = '\0';
		offset = mkoff(line);
		n = mkbuf(buf, line);
		do_cmp(fd, offset, buf, n);
	}

	rc = EXIT_SUCCESS;
	fclose(fp);
	close(fd);

	return rc;
}

static off_t
mkoff(char* line)
{
	char* end;
	long offset;
	int rc = EXIT_FAILURE;

	errno = 0;
	offset = strtol(line, &end, 10);
	if (offset == 0 && (errno == EINVAL || errno == ERANGE))
		err(rc, "bad offset in line `%s'", line);
	if (offset < 0 || *end != ' ')
		errx(rc, "bad offset in line `%s'", line);

	return (off_t)offset;
}

static int
mkbuf(char* buf, char* line)
{
	char* p, *end;
	int n = 0, rc = EXIT_FAILURE;

	if ((p = strchr(line, ' ')) == NULL)
		errx(rc, "bad check bytes in line `%s'", line);

	end = line + strlen(line);
	for (n = 0; *p && p < end && n < MAX_READ; n++) {
		unsigned int ch;

		if (sscanf(p, " %02x", &ch) != 1)
			errx(rc, "bad check bytes in line `%s'", line);
		buf[n] = (unsigned char)ch;
		p += 3;
	}

	return n;
}

static void
do_cmp(int fd, off_t offset, char* buf, int buflen)
{
	char tmp[DEV_BSIZE];
	int rc = EXIT_FAILURE;

	if (pread(fd, tmp, DEV_BSIZE, offset) != DEV_BSIZE)
		err(rc, "pread at `%lu' failed.", offset);
	if (memcmp(tmp, buf, buflen) != 0)
		warnx("offset %lu: mismatch", offset);
}

static void
usage(FILE* fp)
{
	fprintf(fp, "Usage: %s cmd args\n", prog);
	fprintf(fp, "\tcreate dev nsect nread\n"
		    "\tdev    - device/file to read\n"
		    "\tnsect  - no. of sectors in device\n"
		    "\tnread  - no. of reads to make\n"
		    "\n"
		    "\tcheck dev ckfile\n"
		    "\tdev    - device/file to check\n"
		    "\tckfile - check file to use\n"
		    "\n");
}
