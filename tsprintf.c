#include <stdio.h>
#include <stdlib.h>
#include <string.h>

			/*	            1111111111 */
			/*	 01234567890123456789 */

#define NFS_SILLYNAME		".nfsxxxxxxxx_xxxxx_a"
#define NFS_SILLYNAME_INO_OFF	4
#define NFS_SILLYNAME_INO_LEN	8
#define NFS_SILLYNAME_PID_OFF	13
#define NFS_SILLYNAME_PID_LEN	5
#define NFS_SILLYNAME_UNIQ_OFF	18

struct foo_t {
	char s_name[20 * 2];
	long s_namlen;
};


static const char *
print_unknown_value(unsigned int val)
{
	static char str[10][sizeof("UNKNOWN[0x") + (sizeof(int) * 2) + 1];
	static int nxt;
	int oidx;

	oidx = nxt++;
	if (nxt >= 10)
		nxt = 0;

	sprintf(str[oidx], "UNKNOWN[0x%X]", val);

	return str[oidx];
}

int main(void);

int
main()
{
	short pid = 3245;
	struct foo_t foo;
	struct foo_t *sp;

	sp = &foo;

	memcpy(sp->s_name, NFS_SILLYNAME, sizeof(NFS_SILLYNAME));
	sp->s_namlen = sizeof(NFS_SILLYNAME) - 1;
	/* if this isn't guaranteed unique then add in the va_fsid too */
	sprintf(&sp->s_name[NFS_SILLYNAME_INO_OFF], "%0*lX", NFS_SILLYNAME_INO_LEN, (unsigned long) 896745/* np->n_vattr->va_fileid */);
	sp->s_name[NFS_SILLYNAME_INO_OFF + NFS_SILLYNAME_INO_LEN] = '_';
	sprintf(&sp->s_name[NFS_SILLYNAME_PID_OFF], "%0*X", NFS_SILLYNAME_PID_LEN, pid);
	sp->s_name[NFS_SILLYNAME_PID_OFF + NFS_SILLYNAME_PID_LEN] = '_';

	printf("s_name = '%s'\n", sp->s_name);
	printf("strlen(s_name) = %d\n", strlen(sp->s_name));
	printf("s_namlen = %ld\n", sp->s_namlen);

	printf("unknown values: %s\n",
	       print_unknown_value(0x0));
	printf("unknown values: %s %s\n",
	       print_unknown_value(0xffffffff),
	       print_unknown_value(0xeeeeeeee));

	/*
	 * xxx the order of param eval will determine whether first two
	 * overwrite last two, or the last two overwrite the first two
	 */
	printf("unknown values: %s %s %s %s %s %s %s %s %s %s %s %s\n",
	       print_unknown_value(0x11111111),
	       print_unknown_value(0x22222222),
	       print_unknown_value(0x33333333),
	       print_unknown_value(0x44444444),
	       print_unknown_value(0x55555555),
	       print_unknown_value(0x66666666),
	       print_unknown_value(0x77777777),
	       print_unknown_value(0x88888888),
	       print_unknown_value(0x99999999),
	       print_unknown_value(0xaaaaaaaa),
	       print_unknown_value(0xbbbbbbbb),
	       print_unknown_value(0xcccccccc));

	exit(0);
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm " fn "; " (default-value 'compile-command) " CPPFLAGS='-Dfull_main -DCONFORMING' " fn " && ./" fn))
 * End:
 */
