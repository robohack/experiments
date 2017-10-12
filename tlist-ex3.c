/*
 * Example 3
 *
 * And the final example shows how a list of lists can be constructed:
 */

#include <stdio.h>
#include "tlist.h"

int main()
{
  int i, j;
  TLISTDEF(int, tintlist);
  TLIST(tintlist, l5);
  for (i=1; i<10; i++)
  {
    TLISTITER(l5) li = TLISTPUSHBACK(l5, (tintlist){0});
    TLISTINIT(TLISTREF(li));
    for (j=1; j<10; j++)
      TLISTPUSHBACK(TLISTREF(li), i*j);
  }
  TLISTFOREACH(i, l5, ({
    TLISTFOREACH(j, TLISTREF(i), ({
      printf("%d\t", TLISTREF(j));
    }));
    printf("\n");
  }));
  TLISTFOREACH(i, l5, ({ TLISTCLEAR(TLISTREF(i));}));
  TLISTCLEAR(l5);
  return 0;
}

/*
 *
The example uses two new macros TLISTDEF and TLISTINIT. For details of their definitions please refer Appendix.

The output of the above program should be

1       2       3       4       5       6       7       8       9
2       4       6       8       10      12      14      16      18
3       6       9       12      15      18      21      24      27
4       8       12      16      20      24      28      32      36
5       10      15      20      25      30      35      40      45
6       12      18      24      30      36      42      48      54
7       14      21      28      35      42      49      56      63
8       16      24      32      40      48      56      64      72
9       18      27      36      45      54      63      72      81

*/
