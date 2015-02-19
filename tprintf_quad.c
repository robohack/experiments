#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main(void);

int
main()
{
	quad_t l = INT64_MAX;

	/*
	 * these:
	 * gcc (GCC) 4.1.3 20080704 prerelease (NetBSD nb3 20111107)
	 * gcc 4.4.3
	 * say the following on x86_64 (__LP64__):
	 * warning: format '%qu' expects type 'long long unsigned int', but argument 2 has type 'long unsigned int'
	 */
	printf("        1 = (%%qu)%qu\n", (u_quad_t) 1);		/* error on alpha because of XDR bogosity */
	printf("INT64_MAX = (%%qu)%qu\n", (u_quad_t) l);
	printf("        1 = (%%qu)%qu\n", (long long unsigned int) 1);
	printf("INT64_MAX = (%%qu)%qu\n", (long long unsigned int) l);
	printf("        1 = (%%qu)%qu\n", (u_int64_t) 1);		/* error on alpha, of course */
	printf("INT64_MAX = (%%qu)%qu\n", (u_int64_t) l);
	printf("        1 = (%%qu)%qu\n", (long long unsigned int) 1);
	printf("INT64_MAX = (%%qu)%qu\n", (long long unsigned int) l);
	printf("\n");
	printf("        1 = (%%" PRId64 ")%" PRId64 "\n", (u_quad_t) 1);
	printf("INT64_MAX = (%%" PRId64 ")%" PRId64 "\n", (u_quad_t) l);
	exit(0);
	/* NOTREACHED */
}


/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
