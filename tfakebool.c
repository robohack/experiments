void exit(int);				/* avoid #includes */

typedef int mybool;			/* this avoids problems with bool args */
typedef unsigned int myubool;		/* this avoids problems with bool bitfields */

static void
fake_bool_test(mybool t1)
{
	if (t1) {
		exit(0);
	}

	return;
}

static void
fake_ubool_test(myubool t2)
{
	if (t2) {
		exit(0);
	}

	return;
}

struct bitflags {
	unsigned int ui1:1;
	unsigned int ui2:1;
	mybool b1:1;
	mybool b2:1;
	myubool ub1:1;
	myubool ub2:1;
};

void
mytest(int, unsigned int);

void
mytest(int t1,
       unsigned int t2)
{
	struct bitflags bf = {
		t1,	/* clang:  warning: implicit conversion changes signedness: 'int' to 'unsigned int' [-Wsign-conversion] */
		t2,
		t1,
		t2,	/* clang:  warning: implicit conversion changes signedness: 'unsigned int' to 'mybool' (aka 'int') [-Wsign-conversion] */
		t1,	/* clang:  warning: implicit conversion changes signedness: 'int' to 'myubool' (aka 'unsigned int') [-Wsign-conversion] */
		t2,
	};

	fake_bool_test(bf.b1);
	fake_ubool_test(bf.b2);	/* gcc:  warning: passing arg 1 of `fake_ubool_test' as unsigned due to prototype */
				/* clang: warning: implicit conversion changes signedness: 'mybool' (aka 'int') to 'myubool' (aka 'unsigned int') [-Wsign-conversion] */

	return;
}
