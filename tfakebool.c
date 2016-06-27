void exit(int);				/* avoid #includes */

typedef int mybool;
typedef unsigned int myubool;

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
	unsigned int b1:1;
	unsigned int b2:1;
};

void
mytest(int, int);

void
mytest(int t1,
       int t2)
{
	struct bitflags bf = {t1,
	                      t2};

	fake_bool_test(bf.b1);
	fake_ubool_test(bf.b2);

	return;
}
