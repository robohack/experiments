#! /bin/ksh

BUILD_FLAGS="CFLAGS=\"-O2 -W\""
BUILD_DEBUG="\"-Werror -g\""

if false; then
	DQ="'"	# used to wrap multi-token params
	DEBUG_ECHO="echo"
else
	DQ=""
	DEBUG_ECHO=""
fi

# these two are OK with sh and ksh
#
${DEBUG_ECHO} \
sed -e ${DQ}'s/^/to test/'${DQ} \
    -e ${DQ}"s/$/build with:  ${BUILD_FLAGS} but leave ${BUILD_DEBUG} out/"${DQ} < /dev/null
${DEBUG_ECHO} \
sed -e ${DQ}'s/^/to test/'${DQ} \
    -e ${DQ}"s/$/build with:  ${BUILD_FLAGS} but leave ${BUILD_DEBUG:-${BUILD_DEBUG}} out/"${DQ} < /dev/null

# this one fails with sh but works with ksh
#
${DEBUG_ECHO} \
sed -e ${DQ}'s/^/to really test/'${DQ} \
    -e ${DQ}"s/$/build with:  ${BUILD_FLAGS}${BUILD_DEBUG:+%and ${BUILD_DEBUG}}/"${DQ} < /dev/null
