#include <stdio.h>
#include <time.h>

int
main()
{
	char buf[BUFSIZ + 1];
	time_t t;

	(void) time(&t);

	strftime(buf, BUFSIZ, "%Y/%m/%d-%H:%M:%S", localtime(&t));
	printf("%%Y/%%m/%%d-%%H:%%M:%%S = %s\n", buf);

	strftime(buf, BUFSIZ, "%c", localtime(&t));
	printf("%%c = %s\n", buf);

	strftime(buf, BUFSIZ, "%X", localtime(&t));
	printf("%%X = %s\n", buf);

	strftime(buf, BUFSIZ, "%x", localtime(&t));
	printf("%%x = %s\n", buf);

	strftime(buf, BUFSIZ, "%A (%a), %B (%b) %d", localtime(&t));
	printf("%%A (%%a), %%B (%%b) %%d = %s\n", buf);

	exit(0);
	/* NOTREACHED */
}
