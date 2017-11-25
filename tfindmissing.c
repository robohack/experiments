#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/*
 * Given an array of sorted integers, find out which one is missing.
 *
 * All arrays have consecutive integers - and (may) have one missing number
 */

int findMissing(int [], int);

#define BINARY_SEARCH

#ifdef LINEAR_SEARCH
int
findMissing(int arr[],
	    int len)
{
	size_t i;
	int tmp = arr[0];

	for (i = 1; i < len; i++) {
		if (arr[i] > tmp+1) {
			return tmp+1;
		}
		tmp = arr[i];
	}
	return 0;
}
#endif

#ifdef BINARY_SEARCH
/*
 * binary search using recursion
 */
int
findMissing(int arr[],
	    int len)
{
	int i=0, mid = len / 2;

	if (arr[len-1] == arr[0] + (int) (len-1)) {
		/* no elements missing at all! */
		return 0;
	}
	if (len <= 3) {
		if (arr[0]+1 != arr[1]) {
			return arr[0]+1;
		} else if (len == 3 && arr[1]+1 != arr[2]) {
			return arr[1]+1;
		} else
			return 0;
	}
	if (arr[mid-1] == arr[0] + mid-1) {
		/*
		 * there are no elements missing in first half so point the
		 * start index to the second half
		 */
		i = mid-1;
	}
	/* one way or another we've eliminated 1/2 of the array */
	len -= len/2;

	/* so, check the remaining half */
	return(findMissing(arr+i, len));
}
#endif

/*
 * sample arrays for testing...
 *
 * (note because they are arrays of int they cannot be more than MAXINT
 * elements in size)
 */
int a[] = { 4,5,7,8,9 };	int aa = 6;
int b[] = { 4,5,6,8,9,10};	int ba = 7;
int c[] = { 4,6,7,8,9,10};	int ca = 5;
int d[] = { 4,5,6,7,9,10};	int da = 8;
int e[] = { 4,5,6,7,8,9};	int ea = 0;

#define TestIt(an)	assert(findMissing(an, sizeof(an) / sizeof(an[0])) == an ## a)

int main(void);

int
main()
{
	TestIt(a);
	TestIt(b);
	TestIt(c);
	TestIt(d);
	TestIt(e);

	printf("\nAll tests passed\n");

	exit(0);
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm " fn "; " (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
