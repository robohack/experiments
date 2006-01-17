#include <errno.h>
#include <netdb.h>
#include <stdio.h>

/*
 * This is a silly little test program
 */

int
main(argc, argv)
	int	argc;
	char	*argv[];
{
	struct servent *serv;
	struct protoent *proto;
	char **alias;

	printf("%-24s %-11s %s\n", "# Service Name", "Port-#/Proto", "Aliases");

	setservent(1);

	while ((serv = getservent()) != NULL) {
		printf("%-24s %6u/%3s", serv->s_name, (unsigned int) ntohs(serv->s_port), serv->s_proto);
		for (alias = serv->s_aliases; *alias; alias++)
			printf(" %s", *alias);
		putchar('\n');
	}
	endservent();

	setservent(0);

	printf("\nTEST by port: (without proto)\n");
	if (serv = getservbyport((int) htons(6000), (char *) NULL)) {
		printf("%-24s %6u/%3s", serv->s_name, (unsigned int) ntohs(serv->s_port), serv->s_proto);
		for (alias = serv->s_aliases; *alias; alias++)
			printf(" %s", *alias);
		putchar('\n');
	} else
		printf("6000 NOT FOUND.\n");


	printf("\nTEST by port: (with proto='tcp')\n");
	if (serv = getservbyport((int) htons(6000), "tcp")) {
		printf("%-24s %6u/%3s", serv->s_name, (unsigned int) ntohs(serv->s_port), serv->s_proto);
		for (alias = serv->s_aliases; *alias; alias++)
			printf(" %s", *alias);
		putchar('\n');
	} else
		printf("6000 NOT FOUND.\n");


	printf("\nTEST by name: (without proto)\n");
	if (serv = getservbyname("x11", (char *) NULL)) {
		printf("%-24s %6u/%3s", serv->s_name, (unsigned int) ntohs(serv->s_port), serv->s_proto);
		for (alias = serv->s_aliases; *alias; alias++)
			printf(" %s", *alias);
		putchar('\n');
	} else
		printf("x11 NOT FOUND: %s\n", strerror(errno));


	printf("\nTEST by name: (with proto='tcp')\n");
	if (serv = getservbyname("x11", "tcp")) {
		printf("%-24s %6u/%3s", serv->s_name, (unsigned int) ntohs(serv->s_port), serv->s_proto);
		for (alias = serv->s_aliases; *alias; alias++)
			printf(" %s", *alias);
		putchar('\n');
	} else
		printf("x11 NOT FOUND: %s\n", strerror(errno));

	endservent();

	printf("\n\n%-24s %-11s %s\n", "# Protocol Name", "Proto-#", "Aliases");

	setprotoent(1);

	while ((proto = getprotoent()) != NULL) {
		printf("%-24s %6u", proto->p_name, (unsigned int) proto->p_proto);
		for (alias = proto->p_aliases; *alias; alias++)
			printf(" %s", *alias);
		putchar('\n');
	}
	endprotoent();

	exit(0);
}
