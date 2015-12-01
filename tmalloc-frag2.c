#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SIZE (10*1024*1024)

int main(void);

int
main()
{

	size_t actual_alloc=0;
	size_t max_alloc=0;
	unsigned int x;

	char *bigbuf=NULL;
	size_t bigsize=0;

	for  (x = 0; x < 10000000; x++) {
		if (bigbuf!=NULL) {
			actual_alloc -= bigsize;
			free(bigbuf);
		}

		bigsize = rand() % MAX_SIZE; // alloc random amount up to MAX_SIZE
		bigbuf = (char*)malloc(bigsize);
		memset(bigbuf, 'x', bigsize);
		actual_alloc += bigsize;

		if (actual_alloc > max_alloc) {
			max_alloc = actual_alloc;
		}
		if (x%100==0) {
			printf("alloc = %lu \t max = %lu\n", 
			       (unsigned long) actual_alloc, (unsigned long) max_alloc);

			// max_alloc tends towards 10MB, 
			//  "Real Mem" in activity monitor tends towards 60MB
		}
	}

	exit(0);
}
