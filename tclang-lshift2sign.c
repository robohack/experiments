#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * compile with -DMAKE_SAFE to get working code from Clang 4.2
 *
 * this though is OK:
 * gcc version 4.2.1 (Based on Apple Inc. build 5658) (LLVM build 2336.11.00)
 */

int main(void);
int
main()
{
	{
		unsigned int u;

		u = ~0U;
		printf("u = %u\n", u);	/* UINT_MAX */
	}

	{
		unsigned int u;

		u = 1U << ((sizeof(u) * CHAR_BIT) - 1);
		printf("u = %u\n", u);
	}

	{
		unsigned int u;

		u = ~(~0U >> 1);	/* effectively sets MSB */
		printf("u = %u\n", u);
	}

	{
		signed int i;

		i = (int) (((unsigned int) -1) >> 1); /* INT_MAX */
		printf("i = %i\n", i);
	}

	{
		signed int i;

		i = (int) (1U << ((sizeof(i) * CHAR_BIT) - 1));
		printf("i = %d\n", i);
	}

	{
		signed int i;

		i = (int) ~(~0U >> 1);	/* INT_MIN */
		printf("i = %d\n", i);
	}

	{
		signed int i;

		i = (int) ((unsigned int) 1 << ((sizeof(i) * CHAR_BIT) - 1));
		printf("i = %d\n", i);
	}

	{
		unsigned long int lu;

		lu = 1LU << ((sizeof(lu) * CHAR_BIT) - 1);
		printf("lu = %lu\n", lu);
	}

	{
		unsigned long int lu;

		lu = ~(~0LU >> 1);
		printf("lu = %lu\n", lu);
	}

	{
		unsigned long long int llu;

		llu = 1LLU << ((sizeof(llu) * CHAR_BIT) - 1);
		printf("llu = %llu\n", llu);
	}

	{
		unsigned long long int llu;

		llu = ~(~0LLU >> 1);
		printf("llu = %llu\n", llu);
	}

	{
		unsigned int u;

#if defined(MAKE_SAFE) && defined(__clang_major__) && (__clang_major__ >= 4)
		/* assuming I32 */
		u = 0x80000000;
#else
		/*
		 * code generated for this expression by clang 4.2.0 blows
		 * chunks on x86_64:
		 *
		 *	Program received signal EXC_BAD_INSTRUCTION, Illegal instruction/operand.
		 *
		 * as shown above this seems to be due to the literal '1' being
		 * a signed int.
		 *
		 * $ clang -v
		 * Apple LLVM version 4.2 (clang-425.0.28) (based on LLVM 3.2svn)
		 * Target: x86_64-apple-darwin12.4.0
		 * Thread model: posix
		 */
		u = (unsigned int) (1 << ((sizeof(u) * CHAR_BIT) - 1));
#endif
		printf("u = %u\n", u);
	}

	{
		signed int i = 1;

#if defined(MAKE_SAFE) && defined(__clang_major__) && (__clang_major__ >= 4)
		/* assuming I32 */
		i = (int) 0x80000000;
#else
		i = (int) (1 << ((sizeof(i) * CHAR_BIT) - 1));
#endif
		printf("i = %d\n", i);
	}

	{
		unsigned long int lu;

#if defined(MAKE_SAFE) && defined(__clang_major__) && (__clang_major__ >= 4)
		/* assuming __LP64__ */
		lu = 0x8000000000000000;
#else
		lu = (unsigned long) (1L << ((sizeof(lu) * CHAR_BIT) - 1));
#endif
		printf("lu = %lu\n", lu);
	}

	{
		unsigned long long int llu;

#if defined(MAKE_SAFE) && defined(__clang_major__) && (__clang_major__ >= 4)
		/* assuming __LP64__ */
		llu = 0x8000000000000000;
#else
		llu = (unsigned long long) (1LL << ((sizeof(llu) * CHAR_BIT) - 1));
#endif
		printf("llu = %llu\n", llu);
	}

	exit(0);
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm " fn "; " (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
