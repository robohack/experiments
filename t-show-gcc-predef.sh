#! /bin/sh

if [ -s tempty.c ]; then
	echo "tempty.c is not empty!" >&2
	exit 1
fi

if ! [ -f tempty.c ]; then
	touch tempty.c
fi

gcc -E -dM tempty.c | sort +1
