/*
 * The Baudot Code was originally patented by Émile Baudot in 1872.  It defines
 * one character control sequence: "delete", which lives on in our terminals
 * (i.e. terminal emulators) as ASCII character DEL (0x7F).
 *
 * Baudot's name also gives us the "baud" rate that this little C funcion parses.
 *
 * One continuous connection from the now distant past to the present!
 */
int
parse_baudrate(int requested)
{
	int baudrate;

	switch (requested) {
#ifdef B0
	case 0: baudrate = B0; break;
#endif
#ifdef B50
	case 50: baudrate = B50; break;
#endif
#ifdef B75
	case 75: baudrate = B75; break;
#endif
#ifdef B110
	case 110: baudrate = B110; break;
#endif
#ifdef B134
	case 134: baudrate = B134; break;
#endif
#ifdef B150
	case 150: baudrate = B150; break;
#endif
#ifdef B200
	case 200: baudrate = B200; break;
#endif
#ifdef B300
	case 300: baudrate = B300; break;
#endif
#ifdef B600
	case 600: baudrate = B600; break;
#endif
#ifdef B1200
	case 1200: baudrate = B1200; break;
#endif
#ifdef B1800
	case 1800: baudrate = B1800; break;
#endif
#ifdef B2400
	case 2400: baudrate = B2400; break;
#endif
#ifdef B4800
	case 4800: baudrate = B4800; break;
#endif
#ifdef B9600
	case 9600: baudrate = B9600; break;
#endif
#ifdef B19200
	case 19200: baudrate = B19200; break;
#endif
#ifdef B38400
	case 38400: baudrate = B38400; break;
#endif
#ifdef B57600
	case 57600: baudrate = B57600; break;
#endif
#ifdef B115200
	case 115200: baudrate = B115200; break;
#endif
#ifdef B230400
	case 230400: baudrate = B230400; break;
#endif
#ifdef B460800
	case 460800: baudrate = B460800; break;
#endif
#ifdef B500000
	case 500000: baudrate = B500000; break;
#endif
#ifdef B576000
	case 576000: baudrate = B576000; break;
#endif
#ifdef B921600
	case 921600: baudrate = B921600; break;
#endif
#ifdef B1000000
	case 1000000: baudrate = B1000000; break;
#endif
#ifdef B1152000
	case 1152000: baudrate = B1152000; break;
#endif
#ifdef B1500000
	case 1500000: baudrate = B1500000; break;
#endif
#ifdef B2000000
	case 2000000: baudrate = B2000000; break;
#endif
#ifdef B2500000
	case 2500000: baudrate = B2500000; break;
#endif
#ifdef B3000000
	case 3000000: baudrate = B3000000; break;
#endif
#ifdef B3500000
	case 3500000: baudrate = B3500000; break;
#endif
#ifdef B4000000
	case 4000000: baudrate = B4000000; break;
#endif
	default:
		baudrate = -1;
	}
	return baudrate;
}

/*
 * cp210x_quantise_baudrate
 * Quantises the baud rate as per AN205 Table 1
 */
unsigned int
cp210x_quantise_baudrate(unsigned int baud)
{
        if (baud <= 300)           baud = 300;
        else if (baud <= 600)      baud = 600;
        else if (baud <= 1200)     baud = 1200;
        else if (baud <= 1800)     baud = 1800;
        else if (baud <= 2400)     baud = 2400;
        else if (baud <= 4000)     baud = 4000;
        else if (baud <= 4803)     baud = 4800;
        else if (baud <= 7207)     baud = 7200;
        else if (baud <= 9612)     baud = 9600;
        else if (baud <= 14428)    baud = 14400;
        else if (baud <= 16062)    baud = 16000;
        else if (baud <= 19250)    baud = 19200;
        else if (baud <= 28912)    baud = 28800;
        else if (baud <= 38601)    baud = 38400;
        else if (baud <= 51558)    baud = 51200;
        else if (baud <= 56280)    baud = 56000;
        else if (baud <= 58053)    baud = 57600;
        else if (baud <= 64111)    baud = 64000;
        else if (baud <= 77608)    baud = 76800;
        else if (baud <= 117028)   baud = 115200;
        else if (baud <= 129347)   baud = 128000;
        else if (baud <= 156868)   baud = 153600;
        else if (baud <= 237832)   baud = 230400;
        else if (baud <= 254234)   baud = 250000;
        else if (baud <= 273066)   baud = 256000;
        else if (baud <= 491520)   baud = 460800;
        else if (baud <= 567138)   baud = 500000;
        else if (baud <= 670254)   baud = 576000;
        else if (baud < 1000000)   baud = 921600;
        else if (baud > 2000000)   baud = 2000000;
        return baud;
}
