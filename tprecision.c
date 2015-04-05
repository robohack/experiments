#include <stdlib.h>
#include <stdio.h>
#include <float.h>

#if	defined(FLOAT)
#define	F_T	float
#define	F_EPS	FLT_EPSILON
#elif	defined(DOUBLE)
#define	F_T	double
#define	F_EPS	DBL_EPSILON
#else
#define	F_T	long double
#define	F_EPS	LDBL_EPSILON
#endif

#define	F(x)	((F_T)(x))
/* (1 - (4/x - 1) * x) for x==3 */
#define	NEAR_ZERO(x)	(F(1) - (F(4) / (x) - F(1)) * (x))

int
main (void)
{
	long it;
	F_T t;

	it = strtol("3", NULL, 10);
	t = F(it);

	printf("(%u %d %u %.5e):  compiler = %.5e  run_time = %.5e\n",
		(unsigned int) sizeof(long), FLT_EVAL_METHOD,
		(unsigned int) sizeof(F_T), (double) F_EPS,
		(double) NEAR_ZERO(F(3.0)), (double) NEAR_ZERO(t));

	return (0);
}
