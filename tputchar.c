#include <stdio.h>

main()
{
	unsigned int addr = 0x1234;
	static const char xdigs[16] = "0123456789ABCDEF";

	printf("0x%x=", addr);
	putchar((int) xdigs[((addr >> 12) & 0xF)]);
	putchar((int) xdigs[((addr >> 8) & 0xF)]);
	putchar((int) xdigs[((addr >> 4) & 0xF)]);
	putchar((int) xdigs[(addr & 0xF)]);
	printf("\n");

	addr = 0xFEDC;

	printf("0x%x=", addr);
	putchar((int) xdigs[((addr >> 12) & 0xF)]);
	putchar((int) xdigs[((addr >> 8) & 0xF)]);
	putchar((int) xdigs[((addr >> 4) & 0xF)]);
	putchar((int) xdigs[(addr & 0xF)]);
	printf("\n");
}
