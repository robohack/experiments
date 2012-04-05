#include <sys/cdefs.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int deny_severity;
int allow_severity;

int main(int, char *[]);

/*
 *	test parsing and handling of (IPv4) network addresses
 *
 * uses the inet_net(3) functions instead of inet(3) in order to handle CIDR	
 */ 

int
main(argc, argv)
	int	argc		__unused;
	char	*argv[]		__unused;
{
	in_addr_t host_addr = 0;
	in_addr_t mach_host_addr;
	in_addr_t mask;		/* network number mask (in host cpu order) */
	int mask_bits;		/* must be a signed int to hold -1 error value */
	struct in_addr host_in;
	char*q1, *q2, *q3, *q4;
	unsigned char *p;
	
	if (argc != 2) {
		fprintf(stderr, "Usage: %s dot.ted.qu.ad | dot.ted.cid.r/bits\n", argv[0]);
		exit(2);
	}

	host_addr = 0;		/* must be pre-initialized */
	if ((mask_bits = inet_net_pton(AF_INET, argv[1], (void *) &host_addr, sizeof(host_addr))) == -1) {
		fprintf(stderr, "%s: inet_net_pton(): [%s] is an invalid netspec: %s\n", argv[0], argv[1], strerror(errno));
		exit(1);
	}
	if (mask_bits > 32) {
		/* note this is normally impossible with a correct inet_net_pton() impl. */
		fprintf(stderr, "%s: [%s] has invalid number of mask bits: %d\n", argv[0], argv[1], mask_bits);
		exit(1);
	}
	if (!mask_bits && host_addr) {
		host_in.s_addr = host_addr;
		fprintf(stderr,
			"%s: wildcard [%s] has non-zero bits in net part (%s) -- use 0/0 if you want a real wildcard.\n",
			argv[0], argv[1], inet_ntoa(host_in));
	}
	if (!mask_bits && !host_addr) {
		printf("%s: got a wildcard!\n", argv[0]);
	}
#if 0 /* the old way... */
	if ((host_addr = inet_addr(argv[1])) == INADDR_NONE) {
		fprintf(stderr, "%s: '%s' not a valid IP address.\n", argv[0], argv[1]);
		exit(1);
	}
#endif
	/* 
	 * Note: ((u_int32_t) << 32) is undefined in Standard C,
	 * thus the extra test for mask_bits == 0.
	 */
	mask = (mask_bits == 0) ? 0 : (0xffffffffL << (32 - mask_bits));

	if ((ntohl(host_addr) & mask) == 0) {
		fprintf(stderr, "%s: [%s] has invalid netid of 0.\n", argv[0], argv[1]);
	}

	/*
	 * Unfortunately net & host_addr are in network byte order, but so is
	 * the s_addr field of a struct in_addr (also unfortunately)
	 */
	host_in.s_addr = host_addr;
	printf("%s: %s (netsz = %d, netid = 0x%08x, hostid = 0x%08x):\n",
	       argv[0],
	       inet_ntoa(host_in),		/* back to ascii */
	       mask_bits,			/* netsz */
	       ntohl(host_addr) & mask,		/* netid */
	       ntohl(host_addr) & ~mask);	/* hostid */

	printf("sizeof(struct in_addr) = %lu\n", (unsigned long) sizeof(struct in_addr));

	/*
	 * string manipulation works -- but it's four non-trivial function
	 * calls, and it screws up badly when a CIDR netspec is given....
	 */
	q1 = strtok(argv[1], ".");
	q2 = strtok((char *) NULL, ".");
	q3 = strtok((char *) NULL, ".");
	q4 = strtok((char *) NULL, ".");
	printf("string-manip:\t%s.%s.%s.%s.%s\n", q4, q3, q2, q1, "RBL.DOMAIN");

	/*
	 * here we access the individual bytes of the network-order integer
	 * value using char pointer trick.  This is a rather ugly, though quite
	 * efficient, hack, and is the one preferred by some old-style
	 * programmers.  The programmer is assumed to know the byte order of
	 * course (though here we know that we're dealing with network-order),
	 * and unused high-order bits of each byte must still be explicitly
	 * masked off.
	 */
	p = (unsigned char *) ((void *) &host_addr);

	printf("as-array:\t%d/%u.%u.%u.%u.%s\n",
	       mask_bits,
	       (unsigned int) p[3] & 0xff,
	       (unsigned int) p[2] & 0xff,
	       (unsigned int) p[1] & 0xff,
	       (unsigned int) p[0] & 0xff,
	       "RBL.DOMAIN");

	/*
	 * shifting and masking should always work, is quite easy and obvious
	 * to understand, and it is much less expensive than string
	 * manipulation and may even be less expensive than character array
	 * accesses (except for the explicit ntohl() call used here in this
	 * example for clarity's sake).
	 */
	mach_host_addr = ntohl(host_addr);
	printf("by-shifting:\t%d/%u.%u.%u.%u.%s\n",
	       mask_bits,
	       (unsigned int) ((mach_host_addr) & 0xff),
	       (unsigned int) ((mach_host_addr >> 8) & 0xff),
	       (unsigned int) ((mach_host_addr >> 16) & 0xff),
	       (unsigned int) ((mach_host_addr >> 24) & 0xff),
	       "RBL.DOMAIN");

	exit(0);
	/* NOTREACHED */
}
