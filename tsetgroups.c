#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
                                     
int
main(int argc,
     char *argv[])
{
	int	group_count;
	int	i;
	gid_t	group_list[NGROUPS_MAX];

	memset(group_list, '\0', sizeof(group_list));

	group_count = getgroups(NGROUPS_MAX, group_list);
	printf("getgroups() count = %d\n", group_count);
	for (i = 0; i < group_count; i++)
		printf("%d ", group_list[i]);
	printf("\n");

	if (setgroups(0, (int *) NULL) != 0) {
		printf("setgroups(0, NULL) failed: %s\n", strerror(errno));
		if (setgroups(1, group_list) != 0) {
			printf("setgroups(1, group_list) failed: %s\n", strerror(errno));
			return 1;
		} else
			printf("setgroups(1, grouplist) succeeded\n");
	} else
		printf("setgroups(0, NULL) succeeded\n");

	memset(group_list, '\0', sizeof(group_list));

	group_count = getgroups(NGROUPS_MAX, group_list);
	printf("getgroups() count = %d\n", group_count);
	for (i = 0; i < group_count; i++)
		printf("%d ", group_list[i]);
	printf("\n");

	if (setgid(8) != 0) {
		printf("setgid(8) failed: %s\n", strerror(errno));
		return 1;
	}

	printf("gid = %d\n", getgid());
	memset(group_list, '\0', sizeof(group_list));
	group_count = getgroups(NGROUPS_MAX, group_list);
	printf("getgroups() count = %d\n", group_count);
	for (i = 0; i < group_count; i++)
		printf("%d ", group_list[i]);
	printf("\n");

	memset(group_list, '\0', sizeof(group_list));
	group_list[0] = 0;
	group_list[1] = 1;
	group_list[2] = 2;
	group_list[3] = 3;

	if (setgroups(4, group_list) != 0) {
		printf("setgroups(4, [0,1,2,3]) failed: %s\n", strerror(errno));
	} else
		printf("setgroups(4, [0,1,2,3]) succeeded\n");

	memset(group_list, '\0', sizeof(group_list));

	group_count = getgroups(NGROUPS_MAX, group_list);
	printf("getgroups() count = %d\n", group_count);
	for (i = 0; i < group_count; i++)
		printf("%d ", group_list[i]);
	printf("\n");

	system("id");

	memset(group_list, '\0', sizeof(group_list));

	if (getgroupmembership(getlogin(), getgid(), group_list, NGROUPS_MAX, &group_count) < 0) {
		printf("getgroupmembership(%s, %d, ...) failed: %s\n",
		       getlogin(),
		       getgid(),
		       strerror(errno));
	}
	printf("getgroupmembership() count = %d\n", group_count);
	for (i = 0; i < group_count; i++)
		printf("%d ", group_list[i]);
	printf("\n");

	memset(group_list, '\0', sizeof(group_list));

	group_count = NGROUPS_MAX;
	if (getgrouplist(getlogin(), getgid(), group_list, &group_count) < 0) {
		printf("getgrouplist(%s, %d, ...) failed: %s\n",
		       getlogin(),
		       getgid(),
		       strerror(errno));
	}
	printf("getgroupmembership() count = %d\n", group_count);
	for (i = 0; i < group_count; i++)
		printf("%d ", group_list[i]);
	printf("\n");

	if (initgroups(getlogin(), getgid()) < 0) {
		printf("initgroups(%s, %d) failed: %s\n",
		       getlogin(),
		       getgid(),
		       strerror(errno));
	}

	system("id");

	exit(0);
	/* NOTREACHED */
}

