#include <sys/stat.h>
#include <stdlib.h>
#include <time.h>

struct config_stat {
	struct config_stat *succ;
	char    *fn;			/* name of config file */
	time_t  mtime;			/* time of last mod */
	dev_t   dev;			/* device file resides on */
	ino_t   ino;			/* inode number for file */
} *config_stat_list = NULL;

void
add_config_stat(fn, statp)
	char *fn;			/* name of file */
	struct stat *statp;		/* stat of the file, or NULL */
{
	struct config_stat *new = (struct config_stat *) malloc(sizeof(*new));

	new->fn = strcpy(malloc(strlen(fn)), fn);
	if (statp) {
		new->mtime = statp->st_mtime;
		new->dev = statp->st_dev;
		new->ino = statp->st_ino;
	} else {
		new->mtime = 0;
	}
	new->succ = config_stat_list;
	config_stat_list = new;
}

main()
{
	struct stat st;


	stat("/dev/null", &st);
	
	add_config_stat("/dev/null", &st);
	
	exit(0);
}
