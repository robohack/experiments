#include <stdio.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/file.h>
#include <sys/resource.h>
#include <sys/resourcevar.h>
#include <sys/malloc.h>
#include <sys/pool.h>
#include <sys/proc.h>

struct newpstats {
	struct	rusage p_ru;		/* stats for this proc */
	struct	rusage p_cru;		/* sum of stats for reaped children */
	struct	timeval p_start;	/* starting time */
};

main()
{
	printf("sizeof(struct pstats) = %lu.\n", (unsigned long) sizeof(struct pstats));
	printf("sizeof(struct rusage) = %lu.\n", (unsigned long) sizeof(struct rusage));
	printf("sizeof(struct newpstats) = %lu.\n", (unsigned long) sizeof(struct newpstats));
}
