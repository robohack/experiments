#! /bin/sh

/usr/bin/printf "1 foo to stdout\n"
/usr/bin/printf "1 bar to stderr\n" 1>&2

exec > /tmp/$0.$$

/usr/bin/printf "2 foo to stdout\n"
/usr/bin/printf "2 bar to stderr\n" 1>&2

exec > /dev/tty

/usr/bin/printf "3 foo to stdout\n"
/usr/bin/printf "3 bar to stderr\n" 1>&2

exit 0
