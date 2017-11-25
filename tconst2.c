#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int
myfun(int lines,
	    const int array[][6])
{
	int sum = 0;
	int i, j;

#if 0
	array[0][0] = 1;		/* read-only */
	memset(array, '\0', lines * 6 * sizeof(array[0][0]));
#endif

	for (i = 0; i < lines; i++)
		for (j = 0; j < 6; j++)
			sum += array[i][j];

	return sum;
}

static int
myfun2(int lines,
	     const int (*ap)[6])
{
	int sum = 0;
	int i, j;

#if 0
	ap[0][0] = 1;		/* read-only */
	memset(ap, '\0', lines * 6 * sizeof(ap[0][0]));
#endif

	for (i = 0; i < lines; i++)
		for (j = 0; j < 6; j++)
			sum += ap[i][j];

	return sum;
}

static int
simplefun(const int array[6])
{
	int sum = 0;
	int i;

#if 0
	array[0][0] = 1;		/* read-only */
	memset(array, '\0', lines * 6 * sizeof(array[0][0]));
#endif

	for (i = 0; i < 6; i++)
		sum += array[i];

	return sum;
}

static int
simplefun2(const int ap[])
{
	int sum = 0;
	int i;

#if 0
	ap[0][0] = 1;		/* read-only */
	memset(ap, '\0', lines * 6 * sizeof(ap[0][0]));
#endif

	for (i = 0; i < 6; i++)
		sum += ap[i];

	return sum;
}

static int
callmyfun_bad(void)
{
	int myarray[3][6] = {
		{1,2,3,4,5,6},
		{7,8,9,10,11,12},
		{13,14,15,16,17,18}};

	return myfun(sizeof(myarray)/sizeof(*myarray), myarray); /* incompatible pointer type */
}

static int
callmyfun_good(void)
{
	int myarray[3][6] = {
		{1,2,3,4,5,6},
		{7,8,9,10,11,12},
		{13,14,15,16,17,18}};

	return myfun(sizeof(myarray)/sizeof(*myarray), (const int(*)[6]) myarray);
}

static int
callmyfun_good2(void)
{
	int myarray[3][6] = {
		{1,2,3,4,5,6},
		{7,8,9,10,11,12},
		{13,14,15,16,17,18}};

	return myfun2(sizeof(myarray)/sizeof(*myarray), (const int(*)[6]) myarray);
}

static int
callsimplefun(void)
{
	int myarray[6] = {1,2,3,4,5,6};

	return simplefun(myarray);
}

static int
callsimplefun2(void)
{
	int myarray[6] = {1,2,3,4,5,6};

	return simplefun2(myarray);
}


int main(void);

int
main()
{
	printf("bad = %d\n", callmyfun_bad());
	printf("good = %d\n", callmyfun_good());
	printf("good2 = %d\n", callmyfun_good2());

	printf("simple = %d\n", callsimplefun());
	printf("simple2 = %d\n", callsimplefun2());

	exit(0);
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm " fn "; " (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
