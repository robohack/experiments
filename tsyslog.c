#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>

int	main(int, char **);

int
main(int argc            __attribute__((unused)),
     char *argv[])
{
	char *logmsg = NULL;
	const char *msg = "error string: %m";
	const char *argv0 = "progname";

	argv0 = (argv0 = strrchr(argv[0], '/')) ? argv0 + 1 : argv[0];


	openlog(argv0, LOG_PID|LOG_PERROR, LOG_USER);

	/* showing syslog(3) does not rescan for "%m" */

	errno = EPERM;
	syslog(LOG_DEBUG, "(%s) %s (%s)", getlogin(), "testing", msg);

	asprintf(&logmsg, "(%s) %s (%s)", getlogin(), "testing", msg);
	errno = ENODEV;
	syslog(LOG_DEBUG, logmsg);
	free(logmsg);

	exit(0);
}
