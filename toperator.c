main()
{
	int bar;
	int *foo;

	bar = 10;
	foo = & bar;
	printf("foo = %u, bar = %u, bar * foo = %u\n",
	       (unsigned long) foo,
	       (unsigned long) bar,
	       (unsigned long) (bar * foo));
}
