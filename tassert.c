#include <assert.h>
#include <stdlib.h>

int main(void);

int
main()
{
	assert(1 == 0);

	assert(1 == 0 ^ 0 == 1);	/* GCC is stupid: == has a higher precedence than ^ */

	exit(0);
}
