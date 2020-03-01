#define MAKE_CONFLICT

#ifdef MAKE_CONFLICT
/* n.b.:  this will of course give "error: conflicting types for" foo, strarr, and szarr */
extern const char *foo;			/* defined as "char []" */
#else
extern const char foo[];
#endif

extern const char nuts[];

extern char ok[];
extern const char *str;

#ifdef MAKE_CONFLICT
extern char *szarr;			/* defined as "char [100]" */
extern char strarr[];			/* defined as "char *" */
#else
extern char const szarr[100];
extern const char *strarr;
#endif

void bar1(char *);
void bar2(const char *, void *);
