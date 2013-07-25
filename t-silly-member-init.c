#include <stdio.h>
#include <stdlib.h>

struct Base {
	int x;
	int y;
};

struct Extend {
	union {
		int X;
		struct Base bb;
	} uu;
};

struct AlsoExtend {
	struct Base bb;
	int z;
};

static struct Extend Works = {
	.uu.bb.x = 5,
	.uu.bb.y = 3,
};

/*
 * obvious abuse of "union" rules!
 */
static struct Extend Breaks = {
	.uu.bb.x = 5,
	.uu.bb.y = 3,
	.uu.X = 2
};

static struct AlsoExtend AlsoWorks = {
	.bb.x = 5,
	.bb.y = 3,
	.z = 69
};


int main(void);

int
main()
{
	printf("Works: x:%d y:%d X:%d\n", Works.uu.bb.x, Works.uu.bb.y, Works.uu.X);
	printf("Breaks: x:%d y:%d X:%d\n", Breaks.uu.bb.x, Breaks.uu.bb.y, Breaks.uu.X);
	printf("AlsoWorks: x:%d y:%d z:%d\n", AlsoWorks.bb.x, AlsoWorks.bb.y, AlsoWorks.z);

	exit(0);
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
