void
main(void)
{
	int i = 10;
	int *p;

	p = &i;

	/* insert required code here to see "10,20" output */
	int j = 20;
	p = &j;
	/* end insert */

	func();

	printf("%d,%d",i,*p);

}
void
func(void)
{
	int j = 20;
}
