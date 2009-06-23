#include <stdlib.h>
#include <limits.h>

int                      
parseint(const char *str)
{
        char            *ep;            
        long            lval;
 
        errno = 0;

        lval = strtol(str, &ep, 10);    

        if (str[0] == '\0' || *ep != '\0')
                errx(1, "not a number");

        if (errno == ERANGE) {
                if (lval >= INT_MAX)
                        errx(1, "out of range, maximum is %d", INT_MAX);
                else if (lval <= INT_MIN)
                        errx(1, "out of range, minimum is %d", INT_MIN);
        }

        return (int) lval;
}

int                      
parselong(const char *str)
{
        char            *ep;            
        long            lval;
 
        errno = 0;

        lval = strtol(str, &ep, 10);    

        if (str[0] == '\0' || *ep != '\0')
                errx(1, "not a number");

        if (errno == ERANGE) {
                if (lval >= LONG_MAX)
                        errx(1, "out of range, maximum is %ld", LONG_MAX);
                else if (lval <= LONG_MIN)
                        errx(1, "out of range, minimum is %ld", LONG_MIN);
        }

        return lval;
}
