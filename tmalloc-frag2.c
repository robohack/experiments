#include <stdio.h>
#include <stdlib.h>

#define MAX_SIZE (10*1024*1024)

int main (int argc, const char * argv[])
{

	size_t actual_alloc=0;
	size_t max_alloc=0;

	char *bigbuf=NULL;
	size_t bigsize=0;

	for  (long x=0; x<10000000; x++) {
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
			printf("alloc = %u \t max = %u\n", 
			       (unsigned long)actual_alloc, (unsigned long)max_alloc);

			// max_alloc tends towards 10MB, 
			//  "Real Mem" in activity monitor tends towards 60MB
		}
	}


	exit(0);
}
