#include <sys/cdefs.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>

bool match_ipv4_net(
	const char *ipaddr,	/* address to test against, in text presentation format */
	char *netspec,		/* network specification, in text presentation format */
	bool do_mask_net);	/* also mask the network part before comparing */


/*
 * RFC1918 name	IP address range		largest CIDR block (mask)	# of addresses	classful description
 * 24-bit block	10.0.0.0 – 10.255.255.255	10.0.0.0/8 (255.0.0.0)		16,777,216	single class A
 * 20-bit block	172.16.0.0 – 172.31.255.255	172.16.0.0/12 (255.240.0.0)	1,048,576	16 contiguous class Bs
 * 16-bit block	192.168.0.0 – 192.168.255.255	192.168.0.0/16 (255.255.0.0)	65,536		256 contiguous class Cs
 */

typedef struct ipv4 {
	uint8_t data[4];		/* network order */
} ipv4_t;

enum ipv4_error_e {
	ERROR_IPV4_NOERROR = 0,
	ERROR_IPV4_BYTE_OVERFLOW = -1,
	ERROR_IPV4_INPUT_OVERFLOW = -2,
	ERROR_IPV4_INVALID_SYMBOL = -3,
	ERROR_IPV4_NOT_ENOUGH_INPUT = -4,
	ERROR_IPV4_NOT_AF_INET = -5
};

const char *ipv4_errlist[] = {
	"(no error)",
	"Byte overflow (n>256)",
	"Too many bytes (max=4)",
	"Invalid separator symbol",
	"Not enough bytes (4)",
	"Only AF_INET supported"
};
int	ipv4_nerr = { sizeof ipv4_errlist / sizeof ipv4_errlist[0] };

static const char *
ipv4_error(enum ipv4_error_e err)
{
	err = -err;
	if (err > ipv4_nerr) {
		return "Unknown ipvr_error_e value";
	}
	return ipv4_errlist[err];
}

/*
 * I may have taken the inspiration for this from somewhere, but the source is
 * lost in time.
 *
 * Vixie's version, which is static in NetBSD libc/inet/inet_pton.c, will parse
 * values in hex, octal, and "shorthand".
 */
static enum ipv4_error_e
ipv4_parse(const char *string,
           ipv4_t *result)
{
	bool at_least_one_symbol = false;
	const char *cp = string;
	unsigned char symbol;
	uint8_t byte = 0;
	unsigned int data = 0;		/* must be allowed to overflow 2^8 */
	int netbits = ERROR_IPV4_NOT_ENOUGH_INPUT;

	while (*cp) {
		symbol = (unsigned char) *cp;
		if (isdigit(symbol) != 0) {
			symbol = (unsigned char) (symbol - '0');
			data   = data * 10 + symbol;
			if (data > UINT8_MAX) {
				/* 127.0.0.256 */
				return ERROR_IPV4_BYTE_OVERFLOW;
			}
			at_least_one_symbol = true;
		} else if (symbol == '.') {
			if (byte < 3) {
				if (at_least_one_symbol) {
					result->data[byte] = (uint8_t)data;
					data = 0;
					byte++;
					at_least_one_symbol = false;
				} else {
					/* 127.0..1 */
					return ERROR_IPV4_INVALID_SYMBOL;
				}
			} else {
				/* 127.0.0.1.2 */
				return ERROR_IPV4_INPUT_OVERFLOW;
			}
		} else if (symbol == '/') {
			netbits = atoi(++cp);
			break;
		} else {
			/* 127.* */
			return ERROR_IPV4_INVALID_SYMBOL;
		}
		cp++;
	}
	if (byte == 3) {
		if (at_least_one_symbol) {
			result->data[byte] = (uint8_t) data;

			/* this section borrowed from libc */
			/* If no CIDR spec was given, infer width from net class. */
			if (netbits == ERROR_IPV4_NOT_ENOUGH_INPUT) {
				if (result->data[0] >= 240)	/* Class E */
					netbits = 32;
				else if (result->data[0] >= 224) /* Class D */
					netbits = 4;
				else if (result->data[0] >= 192) /* Class C */
					netbits = 24;
				else if (result->data[0] >= 128) /* Class B */
					netbits = 16;
				else				/* Class A */
					netbits = 8;
			}

			return netbits;
		} else {
			/* 127.0.0. */
			return ERROR_IPV4_NOT_ENOUGH_INPUT;
		}
	} else {
		/*
		 * byte will be always less than 3
		 * 127.0
		 */
		return ERROR_IPV4_NOT_ENOUGH_INPUT;
	}
}

/*
 *	ipv4_t v4;
 *	uint32_t ipaddr;	// in_addr_t
 *
 *	(void) ipv4_parse("10.1.2.3", &v4);
 *	ipaddr = ipv4_to_int(&v4);
 */
static in_addr_t
ipv4_to_int(ipv4_t *v4)
{
	return (in_addr_t) (v4->data[0] << 24 | v4->data[1] << 16 | v4->data[2] << 8 | v4->data[3]);
}

/*
 * N.B.: the libc version parses hex, octal, and decimal.
 */
static int
my_inet_net_pton(int af,		/* xxx only AF_INET supported */
                 const void *netspec,
                 in_addr_t *dst,
                 size_t dstsize)
{
	int netbits = 0;
	ipv4_t v4;

	if (af != AF_INET) {
		return ERROR_IPV4_NOT_AF_INET;
	}

	netbits = ipv4_parse(netspec, &v4);

	if (netbits < 0) {
		return netbits;
	}
	if ((size_t) (netbits / 8) > dstsize) {
		return ERROR_IPV4_INPUT_OVERFLOW;
	}

	*dst = ipv4_to_int(&v4);

	return netbits;
}

bool
match_ipv4_net(
	const char *ipaddr,	/* address to test against, in text presentation format */
	char *netspec,		/* network specification, in text presentation format */
	bool do_mask_net)	/* also mask the network part before comparing,
				 * i.e. allow H.O.S.T/CIDR in the netspec */
{
	in_addr_t dest;		/* ipaddr converted to int in network order */
	in_addr_t net;		/* netspec's addr converted to int in network order */
	in_addr_t mask;		/* network number mask (in host order) */
	int netbits;		/* must be a signed int to hold -1 error value */

	net = 0;		/* net must be pre-initialize */
#if 0
	if ((netbits = inet_net_pton(AF_INET, netspec, (void *) &net, sizeof(net))) == -1) {
		syslog(LOG_ERR, "match_ipv4_net(): [%s] is invalid: %s\n", netspec, strerror(errno));
		return (-1);
	}
#else
	if ((netbits = my_inet_net_pton(AF_INET, netspec, (void *) &net, sizeof(net))) < 0) {
		syslog(LOG_ERR, "match_ipv4_net(): [%s] is invalid: %s\n", netspec, ipv4_error(netbits));
		return (-1);
	}
#endif
	net = ntohl(net);

	if (netbits > 32) {
		/* note this is normally impossible with a correct inet_net_pton() impl. */
		syslog(LOG_ERR, "match_ipv4_net(): [%s] has invalid number of mask bits: %d\n", netspec, netbits);
		return (-1);
	}
	if (!netbits && net) {
		/*
		 * note some inet_net_pton() implementations still use inet_addr()
		 * classful parsing syntax internally and as a result "0/0", for
		 * example, may not work and "0.0.0.0/0" may be needed instead.
		 */
		syslog(LOG_ERR, "match_ipv4_net(): wildcard [%s] has non-zero bits in network part -- use 0/0 if you want a real wildcard.\n", netspec);
		return (-1);
	}
	if (!netbits && !net) {
		syslog(LOG_INFO, "match_ipv4_net(): note: [%s] is a wildcard pattern.\n", netspec);
	}

	/*
	 * Note: ((u_int32_t) << 32) is undefined in Standard C,
	 * thus the extra test for netbits == 0.
	 */
	mask = (in_addr_t) ((netbits == 0) ? 0 : (0xffffffffL << (32 - netbits)));

	if (mask && !(net & mask)) {
		/* should never match anything valid */
		syslog(LOG_INFO, "match_ipv4_net(): warning: [%s] has all zero bits in net part.\n", netspec);
	}
	if (mask && (net & ~mask)) {
		/* was giving a host address for the netspec intentional? */
		syslog(LOG_INFO, "match_ipv4_net(): warning: [%s] has non-zero bits in host part.\n", netspec);
	}

	dest = 0;		/* should be initialized? */
	switch (inet_pton(AF_INET, ipaddr, &dest)) { /* xxx use inet_aton() if available? */
	case 0:
		syslog(LOG_DEBUG, "match_ipv4_net(): [%s] is not parsable.\n", ipaddr);
		return (-1);
	case -1:
		syslog(LOG_DEBUG, "match_ipv4_net(): [%s] is invalid: %s\n", ipaddr, strerror(errno));
		return (-1);
	}
	dest = ntohl(dest);

	return ((dest & mask) == (do_mask_net ? (net & mask) : net));
}
