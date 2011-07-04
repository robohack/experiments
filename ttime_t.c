#include <time.h>

#ifndef PRIdTIME
# if TIME_T_MAX == INT32_MAX
#  define PRIdTIME		PRId32
# elif TIME_T_MAX == INT64_MAX
#  define PRIdTIME		PRId64
# endif
#endif
