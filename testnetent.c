#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>

int
main(argc, argv)
	int	argc;
	char	*argv[];
{
	struct netent *net;
	char **alias;

	extern char *inet_ntoa();
	extern struct in_addr inet_makeaddr();

	printf("%-30s %-5s %-15s %s\n", " Network Name", " Type", " Net Number", "Aliases");
	setnetent(0);
	while ((net = getnetent()) != NULL) {
		printf("%-30s %5d %15s", net->n_name, net->n_addrtype, inet_ntoa(inet_makeaddr(net->n_net, 0)));
		for (alias = net->n_aliases; *alias; alias++)
			printf(" %s", *alias);
		putchar('\n');
	}
	exit(0);
}
