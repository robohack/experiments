#if (__STDC_VERSION__ - 0) >= 199901L
# include <inttypes.h>
#endif
#include <time.h>

#include "config.h"		/* for SIZEOF_* definitions */

#ifndef PRIdTIME
# if SIZEOF_TIME_T == SIZEOF_INT32
#  define PRIdTIME	PRId32
# elif SIZEOF_TIME_T == SIZEOF_INT64
#  define PRIdTIME	PRId64
# endif
#endif
