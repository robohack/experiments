int
main()
{
	char buf[BUFSIZ + 1];
	time_t t;

	(void) time(&t);

	strftime(buf, BUFSIZ, "%x", localtime(&t));

	return 0;
}
