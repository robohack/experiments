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
	char **alias;

	printf("%-24s %-11s %s\n", " Service Name", " Num/Proto", "Aliases");
#ifdef HAVE_SETSERVENT
	setservent(0);
#endif
	while ((serv = getservent()) != NULL) {
		printf("%-24s %6d/%3s", serv->s_name, serv->s_port, serv->s_proto);
		for (alias = serv->s_aliases; *alias; alias++)
			printf(" %s", *alias);
		putchar('\n');
	}
	endservent();

	setservent(1);
	printf("\nTEST:\n");
	if (serv = getservbyport(6000, (char *) NULL)) {
		printf("%-24s %6d/%3s", serv->s_name, serv->s_port, serv->s_proto);
		for (alias = serv->s_aliases; *alias; alias++)
			printf(" %s", *alias);
		putchar('\n');
	} else
		printf("6000 NOT FOUND.\n");
	endservent();
	exit(0);
}
