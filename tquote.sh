#! /bin/sh

TARGET_MACHINE=foo
MACHINE_DESCR="The Foo!"

BUILD_FLAGS="CFLAGS=\"-O2 -g\""
BUILD_LDSTATIC="ld -static"

REDIRFILE=~/tmp/netbsd-${TARGET_MACHINE}-case.ps
if ! ${DEBUG}; then
	exec > ${REDIRFILE}
	SHOWREDIR=""
else
	SHOWREDIR="> ${REDIRFILE}"
fi

DQ=""

${DEBUG_ECHO} \
cdlabelgen -l 0 -t cd-tmplt-av8931.ps -e netbsd-all-logos.eps -E netbsd-toaster-logo.eps -T 1.0,-0.1,0.0 -c NetBSD/${TARGET_MACHINE} -s ${DQ}'Planix, Inc.  netbsd-1-6'${DQ} -i ${DQ}"NetBSD/${TARGET_MACHINE}-1.6.2_STABLE for ${MACHINE_DESCR}%%Bootable Binary Installation CD%%Built with:%${BUILD_FLAGS}${BUILD_LDSTATIC:+%and \"${BUILD_LDSTATIC}\"}%%%%For more information see:%%http://www.NetBSD.org/                        http://www.planix.ca/"${DQ} -v 20 -d ${START_DATE} ${SHOWREDIR}
