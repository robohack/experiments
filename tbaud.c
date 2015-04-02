static tcflag_t parse_baudrate(int requested)
{
  int baudrate;

  switch (requested)
    {
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
      baudrate = 0;
    }
  return baudrate;
}
