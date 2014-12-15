extern void boo(int, int, int);

int fun(int, int, int);

int
fun(int a,
    int b,
    int c)
{
	while ((void) (a++ && b++), c--) {
		boo(a, b, c);
	}

	while (a++ ? 0 : b++, c--) {
		boo(a, b, c);
	}
	return a;
}
