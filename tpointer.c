int main(void);

struct memregs {
	unsigned char reg1;
	unsigned char reg2;
};


int
main()
{
	volatile unsigned char * const iop0011 = (unsigned char *)0x0011;
#define ioreg0011 (*iop0011)

	/* the wrong way */
	volatile const unsigned char * bogusp0022 = (unsigned char *)0x0022;
#define bogus0022 (*bogusp0022)

#define memreg0033 (*(unsigned char *)0x0033) /* the simplest way */

#define memstr0044 (*(struct memregs *)0x0044)

	/* either form can be assigned to or read from */
	memreg0033 = ioreg0011;
	ioreg0011 = bogus0022;
	ioreg0011 = memreg0033;

	memstr0044.reg1 = ioreg0011;
	memstr0044.reg2 = memreg0033;
	/* it is an error to try to change what the pointer points to */

	/* it is an error to try to change what the pointer points to */
	// iop0011 = 12;	/* illegal assignment to read-only variable */

	/* showing that bogusp can be changed -- it's not a fixed pointer,
	 * though there is a warning from any decent compiler
	 */
	bogusp0022 = 12;	/* assignment makes pointer from integer without a cast */

	/* but the location it points to cannot be changed as desired!!! */
	// bogus0022 = 12;	/* illegal assignment to read-only location */

	return 0;
}
