#include <stdio.h>
#include <stdlib.h>

#define FAILURE		(-1)
#define SUCCESS		(0)

typedef struct al ArrayList_t;

struct al {
	unsigned int max_size;
	unsigned int current_size;
	int *data;
	int (*append)(ArrayList_t *, int);
	void (*print)(ArrayList_t *);
	void (*insert)(ArrayList_t *, int, int);
	void (*destroy)(ArrayList_t *);
	int (*valueOf)(ArrayList_t *, int);
};


ArrayList_t *
ArrayList_Init(int n,
	       int (*append)(ArrayList_t *, int),
	       void (*print)(ArrayList_t *),
	       void (*insert)(ArrayList_t *, int, int),
	       void (*destroy)(ArrayList_t *),
	       int (*valueOf)(ArrayList_t *, int))
{
	ArrayList_t *newArrayList = (ArrayList_t *) malloc(sizeof(ArrayList_t));

	newArrayList->max_size = n;
	newArrayList->current_size = 0;
	newArrayList->data = malloc(n * sizeof(int));

	newArrayList->append = append;
	newArrayList->destroy = destroy;
	newArrayList->print = print;
	newArrayList->insert = insert;
	newArrayList->valueOf = valueOf;

	return newArrayList;
}

int
append_(ArrayList_t *list,
	int val)
{
	/*
	 * if the array is at maximum capacity
	 *	double the capacity
	 *	update max_size
	 * insert the value in the first open spot in the array (aka index current_size)
	 * increment current_size
	 */
	if (list->current_size == list->max_size) {
		list->max_size *= 2;
		if (!(list->data = realloc(list->data, sizeof(int) * list->max_size)))
			return FAILURE;
	}
	list->data[list->current_size] = val;
	list->current_size++;

	return SUCCESS;
}

void
print_(ArrayList_t *list)
{
	int i;

	printf("List of size %d, max size %d. Contents:\n", list->current_size, list->max_size);

	for (i = 0; i < list->current_size; i++)
		printf("%d, ", list->data[i]);

	printf("\n");
}

void
insert_(ArrayList_t *list,
	int val,
	int index)
{
}

void
destroy_(ArrayList_t *list)
{
}

int
valueOf_(ArrayList_t *list,
	 int index)
{
    return 0;
}

int
main()
{
	ArrayList_t *list;
	int stat, count = 0;

	list = ArrayList_Init(2, append_, print_, insert_, destroy_, valueOf_);

	do {
		printf("Appending %d\n", count);
		stat = list->append(list, count) ; // add val to end of the list
		list->print(list);
	} while (stat == SUCCESS && ++count < 20);

	exit(0);
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm " fn "; " (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
