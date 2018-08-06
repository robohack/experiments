
/*
 * using statement expressions and typeof() to implement a stack-based swap,
 * and doing so without shadowing a usable symbol
 */
#define SWAP_GCC(a, b)	({ typeof(a) SWAP_GCC = a; a = b; b = SWAP_GCC; })


/*
 * A somewhat more portable alternative:
 *
 * N.B.:  the ternary expression assures that 'a' and 'b' have the same type
 */
#define SWAP(a, b)	__swap_internal(&(a), &(b), sizeof(*(1 ? &(a) : &(b))))

#include <string.h>

static inline void
__swap_internal(void *a,
                void *b,
                size_t size)
{
	char tmp[size];

	memcpy(tmp, a, size);
	memmove(a, b, size);
	memcpy(b, tmp, size);

	return;
}

/*
 * oldy-moldy XOR for integer values (usually not faster than a temp register)
 */
void swapXor(int &a,
             int &b)
{
	a ^= b;
	b ^= a;
	a ^= b;
}
