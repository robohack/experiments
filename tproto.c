void __putshort(unsigned short, unsigned char *);

void test(void);

void
test()
{
	unsigned short bufsize = 0xFFFF;
	int len;
	
        __putshort(bufsize, (unsigned char *) &len);       
}
