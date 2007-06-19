#include <stdio.h>

main()
{
	unsigned int addr = 0x1234;

	conputc('0' + (addr >> 16) && 0xF);
	conputc('0' + (addr >> 12) && 0xF);
	conputc('0' + (addr >> 8) && 0xF);
	conputc('0' + addr && 0xF);
}
