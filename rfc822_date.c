#include <stdio.h>
#include <time.h>
#include <locale.h>

char *
rfc822_date(when)
	time_t when;
{
	char *old_locale;
	struct tm *lt;
	struct tm gmt;
	int     gmtoff, end, len;
	static char arpadate[BUFSIZ];

	old_locale = setlocale(LC_ALL, "");
	gmt = *gmtime(&when);
	lt = localtime(&when);
	/* GMT offset calculation adapted from sendmail */
	gmtoff = (lt->tm_hour - gmt.tm_hour) * 60 + lt->tm_min - gmt.tm_min;
	if (lt->tm_year < gmt.tm_year)
		gmtoff -= 24 * 60;
	else if (lt->tm_year > gmt.tm_year)
		gmtoff += 24 * 60;
	else if (lt->tm_yday < gmt.tm_yday)
		gmtoff -= 24 * 60;
	else if (lt->tm_yday > gmt.tm_yday)
		gmtoff += 24 * 60;
	/* %e is not in ANSI X3.159-1989 */
	end = strftime(arpadate, sizeof(arpadate)-1, "%a, %e %b %Y %H:%M:%S ", lt);
	(void) sprintf(&arpadate[end], "%+03d%02d ", (int) (gmtoff / 60), (int) (gmtoff % 60));
	len = strlen(arpadate);
	(void) strftime(&arpadate[len], sizeof(arpadate) - len - 1, "(%Z)", lt);
	(void) setlocale(LC_ALL, old_locale);

	return arpadate;
}
