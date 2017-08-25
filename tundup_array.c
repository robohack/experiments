#include <stdio.h>
#include <stdlib.h>

/*
 * E.g.:
 *
 * Input: {4, 8, 4, 1, 1, 2, 9} Output: {4, 8, 1, 2, 9, ?, ?}
 */

//int numbers[] = {4, 8, 4, 1, 1, 2, 9};
int numbers[] = {4, 6, 8, 4, 1, 1, 2, 9, 6, 5, 9};
//int numbers[] = {1,1,1,1,1};

#define COUNT(n)	(sizeof(n) / sizeof(n[0]))

/*
 * "the naive (N*(N-1)/2) solution"
 *
 * https://stackoverflow.com/a/19333947/816536
 */
static size_t
undup_it(int array[],
         size_t len)
{
	size_t src;
	size_t dst;

	/* an array of size=1 cannot contain duplicate values */
	if (len < 2) {
		return len;
	}
	/* an array of size > 1 will have at least one unique value */
	for (src = dst = 1; src < len; src++) {
		size_t cur;

		for (cur = 0; cur < dst; cur++ ) {
			if (array[cur] == array[src]) {
				break;
			}
		}
		if (cur != dst) {
			continue;	/* found a duplicate */
		}
		/* array[src] must be new: add it to the list of non-duplicates */
		if (dst < src) {	/* avoid copy-to-self */
			array[dst] = array[src];
		}
		dst++;
	}

	return dst; /* number of valid alements in new array */
}

static void
print_it(int array[],
         size_t len)
{
	size_t idx;

	for (idx = 0; idx < len; idx++)  {
		printf("%c %d", (idx) ? ',' :'{' , array[idx] );
	}
	puts("}" );
}

int
main(void)
{
	size_t cnt = COUNT(numbers);

	printf("Before:" );
	print_it(numbers, cnt);

	cnt = undup_it(numbers, cnt);

	printf("After:" );
	print_it(numbers, cnt);

	exit(0);
}
