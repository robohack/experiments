#include <stdio.h>
main ()
{
  /* Are we little or big endian?  From Harbison&Steele.  */
  union
  {
    long l;
    char c[sizeof (long)];
  } u;
  u.l = 1;
  printf("%s\n", (u.c[sizeof (long) - 1] == 1) ? "big" : "little");
  exit(0);
}
