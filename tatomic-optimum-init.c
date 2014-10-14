/*
 * How to safely test for an initialized value without setting the lock first
 *
 * http://ridiculousfish.com/blog/posts/barrier.html
 *
 * http://www.cs.wustl.edu/~schmidt/editorial-3.html
 */
int *
getSharedObject()
{
	static int *sharedObject = NULL;

	if (! sharedObject) {
		LOCK;
		if (! sharedObject) {
			int *temp = malloc(sizeof(tmp));

			init(&temp);
			OSMemoryBarrier();
			sharedObject = temp;
		}
		UNLOCK;
	}
	/* data dependency memory barrier */
	OSMemoryBarrier();		/* not strictly necessary on PPC or x86 */

	return sharedObject;
}
