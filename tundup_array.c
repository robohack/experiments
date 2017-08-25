#include <stdio.h>
#include <stdlib.h>

int numbers[] = {4, 8, 4, 1, 1, 2, 9};
#define COUNT (sizeof numbers / sizeof numbers[0])

size_t
undup_it(int array[],
         size_t len)
{
	size_t src,dst;

	/* an array of size=1 cannot contain duplicate values */
	if (len <2) {
		return len;
	}
	/* an array of size>1 will cannot at least one unique value */
	for (src = dst = 1; src < len; src++) {
		size_t cur;
		for (cur=0; cur < dst; cur++ ) {
			if (array[cur] == array[src]) break;
		}
		if (cur != dst) continue; /* found a duplicate */

		/* array[src] must be new: add it to the list of non-duplicates */
		if (dst < src) {
			array[dst] = array[src]; /* avoid copy-to-self */
		}
        dst++;
        }
return dst; /* number of valid alements in new array */
}

void print_it(int array[], size_t len)
{
size_t idx;

for (idx=0; idx < len; idx++)  {
        printf("%c %d", (idx) ? ',' :'{' , array[idx] );
        }
printf("}\n" );
}

int main(void) {    
    size_t cnt = COUNT;

    printf("Before undup:" );    
    print_it(numbers, cnt);    

    cnt = undup_it(numbers,cnt);

    printf("After undup:" );    
    print_it(numbers, cnt);

    return 0;
}
share
