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

struct node *
reverse_sll(struct node *h0)
{
	struct node *headOfUnReversedLL = h0;
	struct node *headOfReversedLL = null;
	struct node *nodeToReverse = null;

	while (headOfUnReversedLL != null) {
		headOfReversedLL = nodeToReverse;
		nodeToReverse = headOfUnReversedLL;
		headOfUnReversedLL = headOfUnReversedLL->next;
		nodeToReverse->next = headOfReversedLL;
	}
	headOfReversedLL = nodeToReverse;

	return headOfReversedLL;
}

struct node *
recursive_reverse_sll(struct node *aNode,
                      struct node *prevNode) /* set NULL on first call */
{
	struct node *nextNode = NULL;
	struct node *headOfReversedLL = NULL;

	if (aNode != NULL){
		nextNode = aNode->next;
		aNode->next = prevNode;
		headOfReversedLL = recursive_reverse_sll(nextNode, aNode);
	} else {
		headOfReversedLL = prevNode;
	}

	return headOfReversedLL;
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
