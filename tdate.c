#include <time.h>

main()
{
	time_t	start = 0;
	time_t	end = 0x7fffffffL;

	puts(ctime((time_t *) &start));
	puts(ctime((time_t *) &end));
}
