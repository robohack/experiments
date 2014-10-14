getSharedObject()
{
	static id sharedObject;

	if (! sharedObject) {
		LOCK;
		if (! sharedObject) {
			id temp = [[self alloc] init];

			OSMemoryBarrier();
			sharedObject = temp;
		}
		UNLOCK;
	}
	OSMemoryBarrier();

	return sharedObject;
}
