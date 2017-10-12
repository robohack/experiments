#include <stdio.h>
#include "tlist.h"

int main()
{
  TLIST(int, l1);
  
  TLISTPUSHBACK(l1, 69891);
  TLISTPUSHBACK(l1, 71660);
  TLISTPUSHBACK(l1, 71451);
  TLISTPUSHBACK(l1, 71434);
  TLISTPUSHBACK(l1, 72317);
  TLISTPUSHFRONT(l1, 68233);
  
  printf("size of l1=%d\n", TLISTSIZE(l1));
  TLISTFOREACH(i, l1, ({ printf("%d\n", TLISTREF(i)); }));
  printf("Removing first element\n");
  TLISTERASE(TLISTBEGIN(l1));  
  printf("size of l1=%d\n", TLISTSIZE(l1));
  TLISTFOREACH(i, l1, ({ printf("%d\n", TLISTREF(i)); }));
  
  printf("Removing 71660\n");
  TLISTERASE(TLISTFIND(i, l1, ({ TLISTREF(i) == 71660; })));
  printf("size of l1=%d\n", TLISTSIZE(l1));
  TLISTFOREACH(i, l1, ({ printf("%d\n", TLISTREF(i)); }));
  
  printf("Emptying the list\n");
  TLISTCLEAR(l1);
  printf("size of l1=%d\n", TLISTSIZE(l1));
  TLISTFOREACH(i, l1, ({ printf("%d\n", TLISTREF(i)); }));
  
  return 0;
}

/*

Expected output :

size of l1=6
68233
69891
71660
71451
71434
72317
Removing first element
size of l1=5
69891
71660
71451
71434
72317
Removing 71660
size of l1=4
69891
71451
71434
72317
Emptying the list
size of l1=0

 */
