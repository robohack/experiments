/*
 * If the tens digit of a number is 1, then write "th" after the number.  For
 * example:  13th, 19th, 112th, 3911th.
 *
 * If the tens digit is not equal to 1, then use the following table:
 * If the units digit is:	0	1	2	3	4	5	6	7	8	9
 * write this after the number:	th	st	nd	rd	th	th	th	th	th	th
 *
 * For example:  2nd, 7th, 20th, 23rd, 52nd, 135th, 301st.
 */

enum {
	ORD_SUFF_TH,
	ORD_SUFF_ST,
	ORD_SUFF_ND,
	ORD_SUFF_RD,
};

static const char *ord_suff[] = {
	"th",				/* [ORD_SUFF_TH] */
	"st",				/* [ORD_SUFF_ST] */
	"nd",				/* [ORD_SUFF_ND] */
	"rd"				/* [ORD_SUFF_RD] */
};

/*
 * FYI, this first implementation is approximately 0.5-1.0 nanoseconds faster
 * than the second, on a P4-Xeon 2.4GHz.  :-)
 *
 * It's also the smallest code on x86 and x86_64, with and without optimization.
 *
 * The last implementation, though it reads far more clearly, is also
 * significantly more code.
 */
const char *ordinal1(unsigned int num);

const char *
ordinal1(unsigned int num)
{

	if (num != 0) {
		int rmdr;

		rmdr = num % 100;
		if (rmdr > 9 && rmdr < 21) {
			num = 0;
		} else {
			num %= 10;
			if (num > 3) {
				num = 0;
			}
		}
	}

	return ord_suff[num];
}

const char *ordinal2(unsigned int);

const char *
ordinal2(unsigned int num)
{
	if (num != 0) {
		int hrmdr = num % 100;
		int trmdr = num % 10;

		if (hrmdr - trmdr == 10) {
			num = ORD_SUFF_TH;
		} else {
			if (trmdr > 3) {
				num = 0;
			} else {
				num = trmdr;
			}
		}
	}

	return ord_suff[num];
}

const char * ordinal_suff(unsigned int);

const char *
ordinal_suff(unsigned int num)
{
	if (num == 0)
		return ord_suff[ORD_SUFF_TH];
	if (num % 100 >= 11 && num % 100 <= 13)
		return ord_suff[ORD_SUFF_TH];
	switch (num % 10) {
	case 1:
		return ord_suff[ORD_SUFF_ST];
	case 2:
		return ord_suff[ORD_SUFF_ND];
	case 3:
		return ord_suff[ORD_SUFF_RD];
	}

	return ord_suff[ORD_SUFF_TH];
}

#include <stdio.h>
#include <stdlib.h>

int
main(int argc,
     char *argv[])
{
	unsigned int i;

	(void) (argc && argv);

	for (i = 0; i <= 10000; i++) {
		const char *o1;
		const char *o2;
		const char *o3;

		o1 = ordinal1(i);
		o2 = ordinal2(i);
		o3 = ordinal_suff(i);
#if 0
		printf("ordinal1 = %d%s, ordinal2 = %d%s, ordinal_suff = %d%s\n",
		       i, o1,
		       i, o2,
		       i, o3);
#endif
		if (o1 != o2)
			fprintf(stderr, "Mismatch at %d: o1= %s, o2=%s\n", i, o1, o2);
		if (o1 != o3)
			fprintf(stderr, "Mismatch at %d: o1= %s, o3=%s\n", i, o1, o3);
	}

	exit(0);
}
