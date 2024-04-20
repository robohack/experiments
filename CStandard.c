/*
 * this originally comes from Don Heller <dheller@cse.psu.edu>:
 *
 * https://www.cse.psu.edu/~deh25/cmpsc311/Lectures/Standards/CStandard/CStandard.c
 *
 * See also:
 *
 * https://www.cse.psu.edu/~deh25/cmpsc311/Lectures/Standards/CStandard.html
 */

/* source file   CStandard.c  (this file)
 * include file  CStandard.h
 * explanation   README
 *
 * Which version of the C standard are we using?
 * Print some information to stdout.
 *
 * Mind the indentation!
 *
 * The following are required in the C11 Standard (mandatory macros).
 *   __STDC__			C89	C99	C11	C17	C23
 *   __STDC_HOSTED__			C99	C11	C17	C23
 *   __STDC_VERSION__		(C94)	C99	C11	C17	C23
 * The following are optional in the C11 Standard (environment macros).
 *   __STDC_ISO_10646__			C99	C11
 *   __STDC_MB_MIGHT_NEQ_WC__		C99	C11
 *   __STDC_UTF_16__				C11
 *   __STDC_UTF_32__				C11
 * The following are optional in the C11 Standard (conditional feature macros).
 *   __STDC_ANALYZABLE__			C11
 *   __STDC_IEC_559__			C99	C11
 *   __STDC_IEC_559_COMPLEX__		C99	C11
 *   __STDC_LIB_EXT1__				C11
 *   __STDC_NO_ATOMICS__			C11
 *   __STDC_NO_COMPLEX__			C11
 *   __STDC_NO_THREADS__			C11
 *   __STDC_NO_VLA__				C11
 *
 * The following are required in the C11 Standard (mandatory macros).
 *   __DATE__			C89	C99	C11
 *   __FILE__			C89	C99	C11
 *   __LINE__			C89	C99	C11
 *   __TIME__			C89	C99	C11
 */

#include <math.h>
#include <stdio.h>

#include "CStandard.h"

void
CStandard(void);

void
CStandard()
{
  #if defined (__cplusplus)
    printf("This is C++, version %d.\n", __cplusplus);
      /* The expected values would be
       *   199711L, for ISO/IEC 14882:1998 or 14882:2003
       */

  #elif defined(__STDC__)
    printf("This is standard C.\n");

    #if (__STDC__ == 0)
      printf("  The implementation defines __STDC__=0, so pre-dates ISO C.\n");
    #elif (__STDC__ == 1)
      printf("  The implementation is ISO-conforming.\n");
    #else
      printf("  The implementation is not ISO-conforming.\n");
      printf("    __STDC__ = %d\n", __STDC__);
    #endif

    #if defined(__STDC_VERSION__)
      #if (__STDC_VERSION__ > 202311L)
        printf("This is newer than the last known latest ISO C version.\n");
      #elif (__STDC_VERSION__ == 202311L)
        printf("This is C23.\n");
      #elif (__STDC_VERSION__ > 201710L)
        printf("This is C17 (with some expected C23 extensions).\n");
      #elif (__STDC_VERSION__ == 201710L)
        printf("This is C17.\n");
      #elif (__STDC_VERSION__ > 201112L)
        printf("This is C11 (with some expected C17 extensions).\n");
      #elif (__STDC_VERSION__ == 201112L)
        printf("This is C11.\n");
      #elif (__STDC_VERSION__ > 199901L)
        printf("This is C99 (with some expected C11 extensions).\n");
      #elif (__STDC_VERSION__ == 199901L)
        printf("This is C99.\n");
      #elif (__STDC_VERSION__ > 199409L)
        printf("This is C90 (with some expected C99 extensions).\n");
      #elif (__STDC_VERSION__ == 199409L)
        printf("This is C89/C90 with amendment 1, aka ISO/IEC 9899:1990/AMD1:1995, or C94/C95.\n");
      #else
        printf("This is C89 without amendment 1, but with __STDC_VERSION__, aka ISO/IEC 9899:1990, or ISO C90.\n");
      #endif
        printf("  __STDC_VERSION__ = %ldL\n", __STDC_VERSION__);
    #else /* !defined(__STDC_VERSION__) */
      printf("This is C89.  __STDC_VERSION__ is not defined.\n");
      #if !defined(HUGE_VAL)
        printf("  HUGE_VAL is not defined, though it should be!.\n");
      #endif
    #endif

  #else   /* !defined(__STDC__) */
    printf("This is not standard C.  __STDC__ is not defined.\n");
  #endif

  #if defined(__STDC__) && defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
    #if defined(__STDC_HOSTED__)
      #if (__STDC_HOSTED__ == 1)
        printf("  This is a hosted implementation.\n");
      #elif (__STDC_HOSTED__ == 0)
        printf("  This is a freestanding implementation.\n");
      #else
        printf("  __STDC_HOSTED__ = %d is an unexpected value.\n",
               __STDC_HOSTED__);
      #endif
    #else
      printf("  __STDC_HOSTED__ is not defined.\n");
      printf("    This could indicate hosted or freestanding implementation.\n");
    #endif
  
    #if defined(__STDC_ISO_10646__)
      printf("  The wchar_t values conform to the ISO/IEC 10646 standard (Unicode) as of %ld.\n",
             __STDC_ISO_10646__);
    #else
      printf("  __STDC_ISO_10646__ is not defined.\n");
      printf("    The wchar_t values are implementation-defined.\n");
    #endif

    /* added in C99 Technical Corrigendum 3 */
    #if defined(__STDC_MB_MIGHT_NEQ_WC__)
      #if (__STDC_MB_MIGHT_NEQ_WC__ == 1)
        printf("  Wide and multibyte characters might not have the same codes.\n");
      #else
        printf("  __STDC_MB_MIGHT_NEQ_WC__ = %d is an unexpected value.\n",
               __STDC_MB_MIGHT_NEQ_WC__);
      #endif
    #else
      printf("  __STDC_MB_MIGHT_NEQ_WC__ is not defined.\n");
      printf("    Wide and multibyte characters should have the same codes.\n");
    #endif
  
    #if defined(__STDC_IEC_559__)
      #if (__STDC_IEC_559__ == 1)
        printf("  The floating-point implementation conforms to Annex F (IEC 60559 standard).\n");
      #else
        printf("  __STDC_IEC_559__ = %d is an unexpected value.\n",
               __STDC_IEC_559__);
      #endif
    #else
      printf("  __STDC_IEC_559__ is not defined.\n");
      printf("    The floating-point implementation does not conform to Annex F (IEC 60559 standard).\n");
    #endif

    #if defined(__STDC_IEC_559_COMPLEX__)
      #if (__STDC_IEC_559_COMPLEX__ == 1)
        printf("  The complex arithmetic implementation conforms to Annex G (IEC 60559 standard).\n");
      #else
        printf("  __STDC_IEC_559_COMPLEX__ = %d is an unexpected value.\n",
               __STDC_IEC_559_COMPLEX__);
      #endif
    #else
      printf("  __STDC_IEC_559_COMPLEX__ is not defined.\n");
      printf("    The complex arithmetic implementation does not conform to Annex G (IEC 60559 standard).\n");
    #endif

    #if !defined(INFINITY)
      printf("  INFINITY is not defined, though it should be!.\n");
    #endif
    #if !defined(HUGE_VALF)
      printf("  HUGE_VALF is not defined, though it should be!.\n");
    #endif
    #if !defined(HUGE_VALL)
      printf("  HUGE_VALL is not defined, though it should be!.\n");
    #endif
  #endif

  #if defined(__STDC__) && defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L)
    #if defined(__STDC_UTF_16__)
      #if (__STDC_UTF_16__ == 1)
	printf("  The char16_t values are UTF-16 encoded.\n");
      #else
	printf("  __STDC_UTF_16__ = %d is an unexpected value.\n",
	       __STDC_UTF_16__);
      #endif
    #else
      printf("  __STDC_UTF_16__ is not defined.\n");
      printf("    The char16_t values are implementation-defined.\n");
    #endif

    #if defined(__STDC_UTF_32__)
      #if (__STDC_UTF_32__ == 1)
	printf("  The char32_t values are UTF-32 encoded.\n");
      #else
	printf("  __STDC_UTF_32__ = %d is an unexpected value.\n",
	       __STDC_UTF_32__);
      #endif
    #else
      printf("  __STDC_UTF_32__ is not defined.\n");
      printf("    The char32_t values are implementation-defined.\n");
    #endif

    #if defined(__STDC_ANALYZABLE__)
      #if (__STDC_ANALYZABLE__ == 1)
	printf("  The compiler conforms to the specifications in Annex L (Analyzability).\n");
      #else
	printf("  __STDC_ANALYZABLE__ = %d is an unexpected value.\n",
	       __STDC_ANALYZABLE__);
      #endif
    #else
      printf("  __STDC_ANALYZABLE__ is not defined.\n");
      printf("    The compiler does not conform to the specifications in Annex L (Analyzability).\n");
    #endif

    #if defined(__STDC_LIB_EXT1__)
      printf("  The implementation supports the extensions defined in Annex K (Bounds-checking interfaces) as of %ld.\n",
             __STDC_LIB_EXT1__);
    #else
      printf("  __STDC_LIB_EXT1__ is not defined.\n");
      printf("    The implementation does not support the extensions defined in Annex K (Bounds-checking interfaces).\n");
    #endif

    #if defined(__STDC_NO_ATOMICS__)
      #if (__STDC_NO_ATOMICS__ == 1)
	printf("  The implementation does not support atomic types.\n");
      #else
	printf("  __STDC_NO_ATOMICS__ = %d is an unexpected value.\n",
	       __STDC_NO_ATOMICS__);
      #endif
    #else
      // printf("  __STDC_NO_ATOMICS__ is not defined.\n");
      printf("  The implementation supports atomic types and <stdatomic.h>.\n");
    #endif

    #if defined(__STDC_NO_COMPLEX__)
      #if (__STDC_NO_COMPLEX__ == 1)
	printf("  The implementation does not support complex types.\n");
      #else
	printf("  __STDC_NO_COMPLEX__ = %d is an unexpected value.\n",
	       __STDC_NO_COMPLEX__);
      #endif
      #if defined(__STDC_IEC_559_COMPLEX__)
	printf("  However, __STDC_IEC_559_COMPLEX__ is defined, and it should not be.\n");
      #endif
    #else
      // printf("  __STDC_NO_COMPLEX__ is not defined.\n");
      printf("  The implementation supports complex types and <complex.h>.\n");
    #endif

    #if defined(__STDC_NO_THREADS__)
      #if (__STDC_NO_THREADS__ == 1)
	printf("  The implementation does not support threads.\n");
      #else
	printf("  __STDC_NO_THREADS__ = %d is an unexpected value.\n",
	       __STDC_NO_THREADS__);
      #endif
    #else
      // printf("  __STDC_NO_THREADS__ is not defined.\n");
      printf("  The implementation supports threads and <threads.h>.\n");
    #endif

    #if defined(__STDC_NO_VLA__)
      #if (__STDC_NO_VLA__ == 1)
	printf("  The implementation does not support variable length arrays.\n");
      #else
	printf("  __STDC_NO_VLA__ = %d is an unexpected value.\n",
	       __STDC_NO_VLA__);
      #endif
    #else
      // printf("  __STDC_NO_VLA__ is not defined.\n");
      printf("  The implementation supports variable length arrays.\n");
    #endif

  #endif

  #if defined(__STDC__) && defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201710L)

      /*
       * ISO/IEC 9899:2018, a.k.a. C17/C18 (denote the year of completion and
       * publication respectively), is the current revision of the C
       * standard.
       *
       * C17 is same as C11, except that it bumps the __STDC_VERSION__
       * predefined macro to 201710L, contains several defect reports, and
       * deprecates some features.
       */

  #endif

  #if defined(__STDC__) && defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 202311L)
      /* https://en.cppreference.com/w/c/23 */
  #endif

  printf("\n");
}

#ifdef TEST
#include <stdlib.h>

int
main(void);

int
main()
{
	CStandard();
	exit(0);
}
#endif


/* n.b.:  clang only knows -std=iso9899:199409 for ISO C95 */
/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm -f " fn "; " (default-value 'compile-command) " CPPFLAGS+='-DTEST' CSTD=c11 " fn " && ./" fn))
 * End:
 */
