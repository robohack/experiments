#include <stdio.h>
#include <stdlib.h>

int main(void);

int
main()
{
	int v = 10;
	int r = 0;
	int *pv = &v;
	int av[10] =  { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

	putchar('\n');

	printf("v = %d (== 10)\n", v);
	printf("++v = %d (== 11)\n", ++v);
	printf("v = %d (== 11)\n", v);

	printf("++(*pv) = %d (== 12)\n", ++(*pv));
	printf("v = %d, *pv = %d (== 12)\n", v, *pv);

	printf("(*pv)++ = %d (== 12)\n", (*pv)++);
	printf("v = %d, *pv = %d (== 13)\n", v, *pv);

	r = ++(*pv);

	printf("v = %d, r = %d, *pv = %d (== 14)\n", v, r, *pv);

	pv = av;
	printf("sizeof(*pv) = %zu, sizeof(av) = %zu, pv = av = %p, *pv  = %d\n", sizeof(*pv), sizeof(av), pv, *pv);
	pv++;
	printf("++pv = %p, *pv = %d (== 1)\n", pv, *pv);
	pv += 4;
	printf("pv + 4 = %p, *pv = %d (== 5)\n", pv, *pv);

	pv = 5;
	pv = (int *) 5;

	exit(0);
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm " fn "; " (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
