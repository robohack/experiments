void
main(void)
{
	int i = 10;
	int *p;

	p = &i;

	/* insert required code here to see "10,20" output */
#if SUPER_SILLY
	/* MUST be compiled with gcc on i386 WITHOUT optimization */
	p -= 6;
#else
	int n = 20;
	p = &n;
#endif
	/* end insert */

	func();

	printf("%d,%d",i,*p);

}
void
func(void)
{
	int j = 20;
}
