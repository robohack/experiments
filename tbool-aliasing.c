#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

struct bufr {
	char pr_buf[4096];
};

struct a {
	bool error;
	struct bufr pr_buf;
};

struct status {
	int flags;
	int input;
	float delay;
	char data[32];
};

static void
rdec(void *s)
{
	struct status *p = s;

	printf("%d\n", p->input);
	p->input = 1;
	printf("%d\n", p->input);

	return;
}

int
main(void)
{
	struct a *a = calloc((size_t) 1, sizeof(*a));

	rdec(&a->pr_buf);
	printf("&a->error = %p\n", &a->error);
	printf("&a->pr_buf = %p\n", &a->pr_buf);
	printf("&a->pr_buf.pr_buf = %p\n", &a->pr_buf);
	free(a);

	return 0;
}
