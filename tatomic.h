/*
 * define an integer type 'nonatomic_t' and declares a variable 'x' using it
 *
 * also define an identical-sized type 'testint_t' that will never be qualified
 * as volatile, and the size of which can be controlled at compile time by
 * defining NONATOMIC_TYPE as a base integer type.
 */

#ifdef USE_VOLATILE
# define AS_VOLATILE	volatile
#else
# define AS_VOLATILE	/* empty */
#endif

#ifndef NONATOMIC_TYPE
# define NONATOMIC_TYPE	unsigned int
#endif

typedef NONATOMIC_TYPE testint_t;

typedef AS_VOLATILE testint_t nonatomic_t;


#ifndef DO_DEFINE
# define AS_EXTERN	extern
#else
# define AS_EXTERN	/* empty */
#endif


AS_EXTERN nonatomic_t	x;

void catch_signal(int);
