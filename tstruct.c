#include <sys/cdefs.h>

#include <stdio.h>
#include <stdlib.h>

/* a global root object */
struct node {
	int count;
	char *word;
	struct node *next;
} Node;

struct gnode {
	struct gnode *next;
	struct gnode *prev;
};

static struct gnode *
add_gnode(struct gnode *head,
	  struct gnode *new)
{
	new->prev = NULL;
	new->next = head;

	return new;
}

static struct gnode *
last_node(struct gnode *head)
{
	struct gnode *last;

	for (last = head; last->next;) {
		last = last->next;
	}

	return last;
}

static __inline void *
calloc1(size_t nb)
{
	return calloc((size_t) 1, nb);
}

struct gdata {				/* generic data thing */
	struct gnode node;		/* must be first! */
	struct {			/* un-named struct */
		int mutex;
		int count;
	};
	void (*print_fun)(struct gdata);/* gcc 3.x gives bogus warning */
	char *name;
};

static struct gdata *
test_gn1(int count)
{
	struct gdata *gd;
	struct gdata *gdlhead;

	gd = calloc1(sizeof(*gd));

	gd->count = count;

	gdlhead = gd;

	gd = calloc1(sizeof(*gd));

	gd->count = --count;

	gdlhead = (struct gdata *) add_gnode((struct gnode *) gdlhead,
	                                     (struct gnode *) gd);

	gd = calloc1(sizeof(*gd));

	gd->count = --count;

	gdlhead = (struct gdata *) add_gnode((struct gnode *) gdlhead,
	                                     (struct gnode *) gd);

	return gdlhead;
}

struct pinfo {
	int val1;
	int val2;
};

int
test_pbv(const struct pinfo param);

int
test_pbv(const struct pinfo param)	/* pass by value */
{
	int i;
	int j;

	i = param.val1;
	j = param.val2;

	return i + j;
}

int
test_pbr(const struct pinfo *pparam);

int
test_pbr(const struct pinfo *pparam)	/* pass by reference */
{
	return pparam->val1 + pparam->val2;
}

struct pinfo
get_info(int i, int j);

struct pinfo				/* return by value */
get_info(int i, int j)
{
	struct pinfo bar;

	bar.val1 = i;
	bar.val2 = j;

	return bar;			/* return the whole struct as a value */
}


int main(void);

int
main()
{
	struct node nnode;	/* Note: auto storage is by default zeroed */

	/* structure assignment */
	nnode = Node;

	/* getting an aggregate return value and passing it by value */
	nnode.count = test_pbv(get_info(4, 5));

	/*
	 * automatic anonymous aggregate initialization
	 *
	 * passing a pointer to an anonymous auto-storage struct
	 */
	nnode.count = test_pbr(& (struct pinfo) {2, 3});
	/*
	 * automatic aggregate initialization with named fields
	 *
	 * aka "named parameters"
	 *
	 * passing the auto-storage struct by value
	 */
	nnode.count = test_pbv((struct pinfo) {.val1 = 1, .val2 = 2});

	/*
	 * NOTES:
	 *
	 * - the stack space required to make the calls in each of the previous
	 *   two examples should be identical
	 *
	 * - however if a parameter struct is further passed to one or more
	 *   sub-functions it should, if at all possible and sensible to do so,
	 *   be passed by reference as a pointer, as this means the stack space
	 *   required for calling each of the sub-functions is just that
	 *   required for the pointer.
	 *
	 * - does the latter method have any advantage?  Perhaps, in any case
	 *   where the lifetime of the resulting object may exceed the lifetime
	 *   of the call as it may be less likely to accidentally use it in the
	 *   callee without copying it.
	 *
	 * - such parameter structs can be used to provide extensible backward-
	 *   compatible APIs, but they must be designed such that they can
	 *   always be instantiated via anonymous static allocation within a
	 *   parameters list, as above
	 *
	 * - new _optional_ parameters can thus be added to these structures
	 *   without need to update all callers.
	 *
	 * - However of course if changes are not optional then some existing
	 *   structure member will have to change syntactically (in type and/or
	 *   name) in order to facilitate detection of incompatible calls at
	 *   compile time.  (Here's where some kind of compiler-supported
	 *   versioning might help!)
	 */

	/* test generics */ {
		struct gdata *gdlhead;
		struct gdata *gdlast;

		gdlhead = test_gn1(4);
		gdlast = (struct gdata *) last_node((struct gnode *) gdlhead);
		printf("first count = %d\n", gdlhead->count);
		printf("last count = %d\n", gdlast->count);
	}

	exit(0);
}


/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm " fn "; " (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
