#ifndef CSTANDARD_H
#define CSTANDARD_H

/* source file   CStandard.c
 * include file  CStandard.h  (this file)
 * explanation   README
 *
 * Which version of the C standard are we using?
 * Print some information to stdout.
 *
 * declared or defined by this file:
 *   macro:                HAVE_C99
 *                         HAVE_C11
 *   function prototype:   CStandard()
 *
 * usage:
 *   #include <CStandard.h>
 *
 *   #ifdef HAVE_C99
 *   or
 *   #if HAVE_C99
 *
 * Mind the indentation!
 *
 * Defined values of __STDC_VERSION__
 *   C89		undefined
 *   C89 + amendment 1	199409L
 *   C99		199901L
 *   C11		201112L
 */

#undef HAVE_C99
#undef HAVE_C11

#if defined(__STDC__)
#  if defined(__STDC_VERSION__)
#    if __STDC_VERSION__ >= 201112L
#      define HAVE_C99 1
#      define HAVE_C11 1
#    elif __STDC_VERSION__ >= 199901L
#      define HAVE_C99 1
#    endif
#  endif
#endif

/* As a single expression,
 *
 * #define HAVE_C99 (defined(__STDC__) && defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L))
 * #define HAVE_C11 (defined(__STDC__) && defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L))
 */

/* Another version, in the style used by Solaris; we added some indentation.
 *
 * #if defined(__STDC__)
 * #  if __STDC_VERSION__ - 0 >= 199901L
 * #    define HAVE_C99 1
 * #  endif
 * #endif
 */

void CStandard(void);

#endif

