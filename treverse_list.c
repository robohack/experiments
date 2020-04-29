#include <err.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>

/*
 * singly or doubly linked list node for integers
 */
struct node {
	int data;
	struct node* next;
	struct node* prev;		/* NULL if SLL */
};

/*
 * linked list reversal algorithms
 */

static void
reverse_sll(struct node** headp)
{
	struct node* curr = *headp;
	struct node* next;
	struct node* prev = NULL;

	while (curr != NULL) {
		curr->prev = NULL;	/* convert DLL to SLL */

		next = curr->next;	/* remember the next node */
		curr->next = prev;	/* reverse the current node's pointer */

		/* walk to the next node */
		prev = curr;
		curr = next;
	}
	*headp = prev;

	return;
}

#ifdef __GNUC__				/* aka HAVE_TYPEOF */

# define SWAPP(x, y)							\
	do {								\
		const typeof(*x) _temp = *y;				\
		*y = *x;						\
		*x = _temp;						\
	} while (0)

#else

/* hackety hack:  we assume that x and y are pointers to pointers */
# define SWAPP(x, y)	swapp((void **) x, (void **) y)

static void
swapp(void ** const a,
      void ** const b)
{
	void *t = *b;

	*b = *a;
	*a = t;

	return;
}
#endif

static void
reverse_sll_with_swaps(struct node **headp)
{
	struct node *prev = NULL;
	struct node *curr = *headp;

	while (curr) {
		curr->prev = NULL;	/* convert DLL to SLL */

		SWAPP(&prev, &curr->next);
		SWAPP(&curr, &prev);
	}
	*headp = prev;

	return;
}

/* xxx limited by stack size */
static struct node *
recursive_reverse_sll(struct node *curr,
                      struct node *prev) /* set NULL on first call */
{
	struct node *next;
	struct node *head;

	if (curr != NULL) {
		next = curr->next;
		curr->next = prev;
		curr->prev = NULL;	/* convert DLL to SLL */
		head = recursive_reverse_sll(next, curr);
	} else {
		head = prev;
	}

	return head;
}


static void
reverse_dll(struct node **start)
{
	struct node *curr = *start;

	while (curr) {
		SWAPP(&curr->prev, &curr->next);
		if (!curr->prev) {
			*start = curr;
		}
		curr = curr->prev;
	}

	return;
}


/*
 * more SLL/DLL list helpers
 */

static void
push(struct node** headp,
     int val)
{
	struct node* new = malloc(sizeof(struct node));

	new->data = val;
	new->next = (*headp);
	new->prev = NULL;
	if (*headp)  {
		(*headp)->prev = new;
	}
	(*headp) = new;

	return;
}

static void
printList(struct node* head)
{
	struct node* temp = head;

	while (temp != NULL) {
		printf("%d  ", temp->data);
		temp = temp->next;
		/* xxx verify DLL or indicate SLL */
	}

	return;
}

/*
 * extremely pedantic parameter evaluation
 */
static int
e_strtoi(const char *n)
{
	long int ltmp;
	char *ep;

	errno = 0;
	ltmp = strtol(n, &ep, 0);
	if (ep == n) {
		errx(EX_USAGE,
		     "'%s' is not a valid number",
		     n);
	}
	if (*ep) {
		errx(EX_USAGE,
		     "'%s' has invalid trailing characters",
		     n);
	}
	if (errno != 0) {
		err(EX_USAGE,
		    "'%s' is not convertible",
		    n);
	}
	/* xxx if sizeof(int) == sizeof(long int), the compiler may consider it dead code */
	if (ltmp > INT_MAX) {
		errx(EX_USAGE,
		     "%ld is too large (must be <= %d)",
		     ltmp,
		     INT_MAX);
	}
	if (ltmp < INT_MIN) {
		errx(EX_USAGE,
		     "%ld is too small (must be >= %d)",
		     ltmp,
		     INT_MIN);
	}

	return (int) ltmp;
}


int main(int, char *[]);

/* Driver program to test above function*/
int
main(int argc,
     char *argv[])
{
	/* Start with an empty list */
	struct node* head = NULL;

	if (argc == 1) {
		push(&head, 42);
		push(&head, 7);
		push(&head, 1);
		push(&head, -3);
		push(&head, 98);
	} else {
		int n = 1;

		/*
		 * of course we could cheat here and just walk backwards over
		 * argv[] and print the result immediately!
		 */
		while (n < argc) {
			push(&head, e_strtoi(argv[n++]));
		}
	}

	printf("Given doubly linked list\n");
	printList(head);
	putchar('\n');

	reverse_dll(&head);

	printf("\nReversed doubly linked list\n");
	printList(head);
	putchar('\n');

	reverse_sll(&head);

	printf("\nBack to normal linked list\n");
	printList(head);
	putchar('\n');

	reverse_sll_with_swaps(&head);

	printf("\nReversed linked list with swaps\n");
	printList(head);
	putchar('\n');

	head = recursive_reverse_sll(head, NULL);

	printf("\nBack to normal linked list recursively\n");
	printList(head);
	putchar('\n');

	exit(0);
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm " fn "; " (default-value 'compile-command) " " fn " && ./" fn " 'string to reverse'"))
 * End:
 */
