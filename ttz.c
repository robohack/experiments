#include <stdlib.h>
#include <stdio.h>
#include <time.h>

extern char **environ;

static void doit(time_t now, char *s)
{
struct tm *t = localtime(&now);
char *tz = getenv("TZ");
printf("TZ = %s\n", (tz == NULL)? "<unset>" : tz);
printf("  %04d-%02d-%02d %02d:%02d:%02d  %s\n",
  1900 + t->tm_year, 1 + t->tm_mon, t->tm_mday,
  t->tm_hour, t->tm_min, t->tm_sec, s);
}

int main(void)
{
char *null = NULL;
time_t now = time(NULL);

doit(now, "default");

putenv("TZ=");
tzset();
doit(now, "TZ set to empty string");

putenv("TZ=EST");
tzset();
doit(now, "TZ set to EST");

environ = &null;
tzset();
doit(now, "environment removed");

return 0;
}
