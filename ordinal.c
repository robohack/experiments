
static const char *ord_suff[] = {
	"th",
	"st",
	"nd",
	"rd"
};

/*
 * FYI, this first implementation is approximately 0.5-1.0 nanoseconds faster
 * than the second, on a P4-Xeon 2.4GHz.  :-)
 */
const char *
ordinal1(
	int num
)
{
	int rmdr;

	if (num == 0) {
		return "";
	}

	rmdr = num % 100;
	if (rmdr > 9 && rmdr < 21) {
		num = 0;
	} else {
		num %= 10;
		if (num > 3) {
			num = 0;
		}
	}

	return ord_suff[num];
}

const char *
ordinal2(
	int num
)
{
	int hrmdr = num % 100;
	int trmdr = num % 10;

	if (num == 0) {
		return "";
	}

	if (hrmdr - trmdr == 10) {
		num = 0;
	} else {
		if (trmdr > 3) {
			num = 0;
		} else {
			num = trmdr;
		}
	}

	return ord_suff[num];
}

#include <stdio.h>
#include <stdlib.h>

int
main(
	int argc,
	char *argv[]
)
{
	int i;

	for (i = 0; i <= 10000000; i++) {
#if 0
		printf("ordinal1 = %d%s, ordinal2 = %d%s\n",
		       i, ordinal1(i),
		       i, ordinal2(i));
#else
		const char *x;
		const char *y;

		x = ordinal1(i);
		y = ordinal2(i);
		if (x != y)
			fprintf(stderr, "Mismatch at %d: o1= %s, o2=%s\n", i, x, y);
#endif
	}

	exit(0);
	/* NOTREACHED */
}
