#ifndef __LCC__
# include <stdio.h>
int      printf  (const char *  , ...)  ;
int      vfprintf  (FILE *  , const char *  , char * )  ;
#else
extern int __sF[];
#endif

# include <stdarg.h>

int

printf( const char *fmt, ...)

{
        int ret;
        va_list ap;
	char foo[] = { 'a', 'b', 'c', '\n', '\0' };
	char *bar;

	bar = "def\n";

        (( ap ) = (va_list)__builtin_next_arg(  fmt )) ;

        ret = vfprintf((&__sF[1]) , fmt, ap);
	(void) vfprintf((&__sF[2]) , fmt, ap);

	printf(foo);
	printf(bar);

        return (ret);
}
