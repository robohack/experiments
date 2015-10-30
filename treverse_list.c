reverse(struct node **start)
{
	struct node *prev = NULL;
	struct node *curr = *start;

	while (curr) {
		swap(&prev, &curr->next);
		swap(&curr, &prev);
	}
	*start = prev;
}

reverse_dl(struct node **start)
{
	struct node *curr = *start;

	while (curr) {
		swap(&curr->prev, &curr->next);
		if (!curr->prev)
			*start = curr;
		curr = curr->prev;
	}
}
