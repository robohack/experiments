#! /bin/sh

/usr/bin/printf 1 foo to stdout
/usr/bin/printf 1 bar to stderr 1>&2

exec > /tmp/$0.$$

/usr/bin/printf 2 foo to stdout
/usr/bin/printf 2 bar to stderr 1>&2

exec > /dev/tty

/usr/bin/printf 2 foo to stdout
/usr/bin/printf 2 bar to stderr 1>&2
