//
// void prntnum(unsigned long n, int base, char sign, char *outbuf)
//
// unsigned long num = number to be printed
// int base        = number base for conversion;  decimal=10,hex=16
// char sign       = signed or unsigned output
// char *outbuf   = buffer to hold the output number
//

void
prntnum(unsigned long n,
        int base,
        char sign,
        char *outbuf)
{


	int i = 12;

	int j = 0;


	do{
		outbuf[i] = "0123456789ABCDEF"[num % base];

		i--;

		n = num/base;

	}while( num > 0);


	if(sign != ' '){
		outbuf[0] = sign;

		++j;

	}

	while( ++i < 13){
		outbuf[j++] = outbuf[i];

	}

	outbuf[j] = 0;


}
