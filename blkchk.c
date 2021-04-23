/*
 *	blkchk - read random blocks from a device, then check they are unchanged
 *
 * original version supplied by RVP <rvp@SDF.ORG>
 *
 * Concept:
 *
 * Test if two different machines (e.g. a Xen dom0 and domU; or an NFS srever
 * and client; removable media exchanged between machines, etc.) can read
 * identical data from the same device.
 *
 * This was inspired by a problem I've encountered where a Xen domU running
 * FreeBSD (on a Xen-dom0 running NetBSD-current 9.99.81 as of 2020-06-09) does
 * not read back what it wrote, and for example this means "newfs /dev/da0 &&
 * fsck /dev/da0" fails to fsck cleanly.  Since fsck does scattered reads of
 * various sizes, some with read() and others with pread(), we mimic this, but
 * using purely random offsets and block size.
 *
 * 0. write interesting or random data to a device (optional)
 *
 * 1. run "./blkchk create /dev/dev 8192 100 > chkfile"
 *
 * 2. move the device to another machine, and copy chkfile there too
 *
 * 3. run "./blkchk check /dev/dev chkfile" on 2nd machine to verify it reads OK
 *
 * - initial reads are done with read(2) from a random offset (aligned to
 *   DEV_BSIZE) and in a random range of block sizes up to 65536 bytes and
 *   then written in parseable textual form to stdout
 *
 * - check reads are done with pread(2) and compared to parsed text from 1st run
 *
 * To test this program do something like the following:
 *
 *	jot 0 | dd count=8192 > bfile
 *	./blkchk create bfile 8192 100 > bfile.check
 *	./blkchk check bfile bfile.check
 *	jot 0 0 - -1 | dd count=8192 > badfile
 *	./blkchk check [-v] badfile bfile.check
 */

#include <sys/param.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

static int do_create(int argc, char *argv[]);
static int do_check(int argc, char *argv[]);
static unsigned long parsenum(const char *line, unsigned long ln);
static size_t mkbuf(const char *fname, char *buf, const char *line, unsigned long ln);
static void do_cmp(const char *devame, int fd, off_t offset, const char *fname, unsigned long ln, char *buf, size_t buflen);
static unsigned int time_seed(void);
static long random_rng(long max);
static size_t random_bs(void);
static void usage(FILE *fp);

enum {
	MAX_BS = 65536,			/* maximum size of a read */
};

unsigned int verbose = 0;

int
main(int argc, char *argv[])
{
	int rc = EXIT_FAILURE;
	char *cmd;
	int ch;

	if (argc < 2) {
		usage(stderr);
		exit(rc);
	}

	argc--;
	cmd = argv[1];
	argv[1] = argv[0];		/* xxx maybe not? */
	argv++;

	optind = 1;			/* start at the beginning */
	opterr = 0;			/* disable getopt() from printing errors (returns '?' with optopt set) */
	while ((ch = getopt(argc, argv, ":v")) != -1) {
		switch (ch) {
		case 'v':
			verbose++;
			break;
		case ':':
			/*
			 * NOTE:  a leading ':' in optstring causes getopt() to
			 * return a ':' when an option is missing its parameter.
			 */
			fprintf(stderr, "%s %s: missing parameter for -%c\n", getprogname(), cmd, optopt);
			usage(stderr);
			exit(rc);

		case '?':
			fprintf(stderr, "%s %s: unknown option -%c\n", getprogname(), cmd, optopt);
			usage(stderr);
			exit(rc);

		default:
			fprintf(stderr, "%s %s: programming error, unhandled flag: %c\n", getprogname(), cmd, ch);
			abort();

		}
	}
	argc -= optind;
	argv += optind;

	if (strcmp(cmd, "create") == 0) {
		rc = do_create(argc, argv);
	} else if (strcmp(cmd, "check") == 0) {
		rc = do_check(argc, argv);
	} else if (strcmp(cmd, "help") == 0) {
		usage(stdout);
		rc = EXIT_SUCCESS;
	} else
		warnx("%s: unknown command", cmd);

	exit(rc);
}

static int
do_create(int argc, char *argv[])
{
	char *devname, *s, *end;
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

	srandom(time_seed());

	for (n = 0; n < nread; ) {
		char buf[MAX_BS];
		size_t i;
		size_t bs = random_bs();
		off_t offset = random_rng(nsect - (long) (bs/DEV_BSIZE)) * DEV_BSIZE;

		if (lseek(fd, offset, SEEK_SET) == -1)
			err(rc, "%s: lseek offset `%lu' failed", devname, offset);
		if (read(fd, buf, bs) != (ssize_t) bs)
			err(rc, "%s: read `%lu' bytes (at %lu) failed", devname, (unsigned long) bs, offset);
		printf("%lu", (unsigned long) offset);
		printf(" %lu", (unsigned long) bs);
		for (i = 0; i < bs; i++)
			printf(" %02x", (unsigned char)buf[i]);
		printf("\n");
		n++;
	}

	close(fd);
	rc = EXIT_SUCCESS;

	return rc;
}

static int
do_check(int argc, char *argv[])
{
	char *devname;
	char *fname;
	char line[((20 + 1) * 2) + MAX_BS*3 + 1]; /* 20+1 == log10(2^64)+1 */
	char buf[MAX_BS];
	int fd, rc = EXIT_FAILURE;
	unsigned long ln = 0;
	FILE *fp;

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
		char *p;
		off_t offset = (off_t) parsenum(line, ln);
		size_t n = mkbuf(fname, buf, line, ln);

		if ((p = strrchr(line, '\n')) != NULL)
			*p = '\0';
		do_cmp(devname, fd, offset, fname, ln, buf, n);
		ln++;
	}

	rc = EXIT_SUCCESS;
	fclose(fp);
	close(fd);

	return rc;
}

static size_t
mkbuf(const char *fname,
      char *buf,
      const char *line,
      unsigned long ln)
{
	char *p;
	const char *end;
	size_t bs;
	size_t n;
	int rc = EXIT_FAILURE;

	if ((p = strchr(line, ' ')) == NULL)
		errx(rc, "%s: bad offset bytes in line `%s'", fname, line);

	bs = parsenum(p, ln);
	p++;

	if ((p = strchr(p, ' ')) == NULL)
		errx(rc, "%s: bad bs in line `%s'", fname, line);

	end = line + strlen(line);
	for (n = 0;  p < end && n < bs; n++) {
		unsigned int ch;

		if (sscanf(p, " %02x", &ch) != 1)
			errx(rc, "%s: bad check bytes in line# %lu", fname, ln);
		buf[n] = (char)ch;
		p += 3;
	}
	if (n != bs)
		errx(rc, "%s: wrong number of check bytes in line# %lu", fname, ln);

	return n;
}

static void
do_cmp(const char *devname,
       int fd,
       off_t offset,
       const char *fname,
       unsigned long ln,
       char *buf,
       size_t buflen)
{
	char tmp[MAX_BS];
	int rc = EXIT_FAILURE;
	bool preadp;

	preadp = random() & 1;
	if (preadp) {
		if (pread(fd, tmp, buflen, offset) != (ssize_t) buflen)
			err(rc, "pread(%s, tmp, %d, %lu, %lu, %lu) failed",
			    devname, MAX_BS, offset, (unsigned long) buflen, (unsigned long) offset);
	} else {
		/*
		 * xxx this may not be exactly the same pattern as fsck, which
		 * may do one lseek(), then interleave pread() and read(),
		 * i.e. expecting a following read() to continue where the last
		 * one left off....
		 */
		if (lseek(fd, offset, SEEK_SET) == -1)
			err(rc, "%s: lseek offset `%lu' failed", devname, offset);
		if (read(fd, tmp, buflen) != (ssize_t) buflen)
			err(rc, "read(%s, tmp, %d, %lu, %lu) failed",
			    devname, MAX_BS, offset, (unsigned long) buflen);
	}
	if (memcmp(tmp, buf, buflen) != 0) {
		size_t n;

		for (n = 0; n < buflen; n++) {
			if (tmp[n] != buf[n]) {
				warnx("%s %lu bytes @ %lu: mismatch: %s[+%lu] \\x%02x != %s[ln#%lu][%lu] \\x%02x",
				      preadp ? "pread" : "read",
				      (unsigned long) buflen, offset,
				      devname, n, (unsigned char) tmp[n],
				      fname, ln, n, (unsigned char) buf[n]);
				if (!verbose)
					break;
			}
		}
	}
}

static unsigned long
parsenum(const char *start, unsigned long ln)
{
	char *end;
	unsigned long num;
	int rc = EXIT_FAILURE;

	errno = 0;
	num = strtoul(start, &end, 10);
	if (end == start)
		err(rc, "expected number in line# %lu", ln);
	if (*end != ' ')
		err(rc, "trailing garbage on number in line# %lu", ln);
	if (errno != 0)
		err(rc, "bad number in line# %lu", ln);

	return num;
}

/*
 * Choose and return an initial random seed based on the current time.
 *
 * From https://benpfaff.org/writings/clc/random-seed.html
 * Based on code by Lawrence Kirby <fred@genesis.demon.co.uk>.
 * getpid() added by "Greg A. Woods" <woods@planix.ca>
 *
 * Usage: srand(time_seed());
 */
static unsigned int
time_seed(void)
{
	time_t timeval;			/* Current time. */
	unsigned char *ptr;		/* Type punned pointed into timeval. */
	unsigned seed;			/* Generated seed. */
	size_t i;

	timeval = time((time_t *) NULL) - (time_t) getpid();
	ptr = (unsigned char *) &timeval;

	seed = 0;
	for (i = 0; i < sizeof(timeval); i++) {
		seed = (seed * (UCHAR_MAX + 2u)) + ptr[i];
	}

	return seed;
}

// from:  https://stackoverflow.com/a/6852396
//
// Returns a random (long) integer in the closed interval [0..max]
//
static long
random_rng(long max)		// Assumes 0 <= max <= RAND_MAX
{
	// max <= RAND_MAX < ULONG_MAX, so this is okay.
	unsigned long num_bins = (unsigned long) max + 1;
	unsigned long num_rand = (unsigned long) RAND_MAX + 1;
	unsigned long bin_size = num_rand / num_bins;
	unsigned long defect   = num_rand % num_bins;
	long x;

	do {
		x = random();
	} while (num_rand - defect <= (unsigned long) x);   // This is carefully written not to overflow

	// Truncated division is intentional
	return x / (long) bin_size;
}

static size_t
random_bs(void)
{
	switch (random_rng(8)) {
	default:
	case 0:
		return 512;
	case 1:
		return 1024;
	case 2:
		return 2048;
	case 3:
		return 4096;
	case 4:
		return 8192;
	case 5:
		return 16384;
	case 6:
		return 32768;
	case 7:
		return MAX_BS;		/* all other values must be lower! */
	}
	/* NOTREACHED */
}

static void
usage(FILE *fp)
{
	fprintf(fp, "Usage: %s cmd args\n", getprogname());
	fprintf(fp, "\tcmd    args\n"
		    "\t---    ----\n"
		    "\tcreate dev nsect nread > chkfile\n"
		    "\tdev    - device/file to read\n"
		    "\tnsect  - no. of sectors in device (in units of DEV_BSIZE=%d)\n"
		    "\tnread  - no. of random reads to do\n\n"

		    "\tcheck  dev ckfile\n"
		    "\tdev    - device/file to check\n"
		    "\tckfile - check file to use\n"
		    "\n", DEV_BSIZE);
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm " fn "; " (default-value 'compile-command) " " fn))
 * End:
 */
