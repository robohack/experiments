#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>                        
#include <stdlib.h>                                                        
                                     
int main()                                                  
{                                                                           
int group_count, i;
gid_t group_list[NGROUPS_MAX];

printf("count = %d\n", group_count = getgroups(NGROUPS_MAX, group_list));
for (i = 0; i < group_count; i++) printf("%d ", group_list[i]);
printf("\n");

if (setgroups(0, (int *) NULL) != 0)
  {
  printf("setgroups(0,NULL) failed: %s\n", strerror(errno)); 
  if (setgroups(1, group_list) != 0) 
    {
    printf("setgroups(1, group_list) failed: %s\n", strerror(errno));
    return 1;
    }
  else printf("setgroups(1,grouplist) succeeded\n");   
  }
else printf("setgroups(0,NULL) succeeded\n");      

printf("count = %d\n", group_count = getgroups(NGROUPS_MAX, group_list));
for (i = 0; i < group_count; i++) printf("%d ", group_list[i]);
printf("\n");

if (setgid(8) != 0) 
  {
  printf("setgid(8) failed: %s\n", strerror(errno));
  return 1;
  }

printf("gid = %d\n", getgid());
printf("count = %d\n", group_count = getgroups(NGROUPS_MAX, group_list));
for (i = 0; i < group_count; i++) printf("%d ", group_list[i]);
printf("\n");

return 0;
}

