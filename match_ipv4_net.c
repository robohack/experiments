#include <sys/cdefs.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <errno.h>
#include <string.h>
#include <syslog.h>

int
match_ipv4_net(ipaddr, netspec)
	const char *ipaddr;	/* address to test against, in text presentation format */
	char *netspec;		/* network specification, in text presentation format */
{
	in_addr_t dest;		/* ipaddr converted to int in network order */
	in_addr_t net;		/* netspec's addr converted to int in network order */
	in_addr_t mask;		/* network number mask (in host order) */
	int mask_bits;		/* must be a signed int to hold -1 error value */

	net = 0			/* net must be pre-initialize */
	if ((mask_bits = inet_net_pton(AF_INET, netspec, (void *) &net, sizeof(net))) == -1) {
		syslog(LOG_ERR, "match_ipv4_net(): [%s] is invalid: %s\n", netspec, strerror(errno));
		return (-1);
	}
	if (mask_bits > 32) {
		/* note this is normally impossible with a correct inet_net_pton() impl. */
		syslog(LOG_ERR, "match_ipv4_net(): [%s] has invalid number of mask bits: %d\n", netspec, mask_bits);
		return (-1);
	}
	if (!mask_bits && net) {
		/*
		 * note some inet_net_pton() implementations still use inet_addr()
		 * classful parsing syntax internally and as a result "0/0", for
		 * example, may not work and "0.0.0.0/0" may be needed instead.
		 */
		syslog(LOG_ERR, "match_ipv4_net(): wildcard [%s] has non-zero bits in network part -- use 0/0 if you want a real wildcard.\n", netspec);
		return (-1);
	}
	if (!mask_bits && !net) {
		syslog(LOG_INFO, "match_ipv4_net(): note: [%s] is a wildcard pattern.\n", netspec);
	}

	/* 
	 * Note: ((u_int32_t) << 32) is undefined in Standard C,
	 * thus the extra test for mask_bits == 0.
	 */
	mask = (mask_bits == 0) ? 0 : (0xffffffffL << (32 - mask_bits));

	net = ntohl(net);

	if (mask && !(net & mask)) {
		/* should never match anything valid */
		syslog(LOG_INFO, "match_ipv4_net(): warning: [%s] has all zero bits in net part.\n", netspec);
	}
	if (mask && net & ~mask) {
		/* was giving a host address for the netspec intentional? */
		syslog(LOG_INFO, "match_ipv4_net(): warning: [%s] has non-zero bits in host part.\n", netspec);
	}

	dest = 0;		/* should be initialized? */
	switch (inet_pton(AF_INET, ipaddr, &dest)) {
	case 0:
		syslog(LOG_DEBUG, "match_ipv4_net(): [%s] is not parsable.\n", ipaddr);
		return (-1);
	case -1:
		syslog(LOG_DEBUG, "match_ipv4_net(): [%s] is invalid: %s\n", ipaddr, strerror(errno));
		return (-1);
	}
	dest = ntohl(dest);

	return ((dest & mask) == (net & mask));
}
