#! /bin/sh

CC=${CC:-"cc"}

${CC} -E -dM -x c /dev/null | sort

# to show optimizer options (and include path, etc.):
#
OPTIM="-O4"
OPTIM="-O3"
OPTIM="-O2"
OPTIM="-O1"
OPTIM="-O0"
#
#${CC} -v ${OPTIM} -S -fverbose-asm -o - -x c /dev/null
