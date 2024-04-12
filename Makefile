# -*- makefile-bsdmake -*-
#
#	Makefile - of the BSD variety
#
# (on older Mac OS X, use "bsdmake"; on macOS install "bmake")
#
# Use "LDLIBS+=-llibrary" or "LDLIBS+='-L/usr/pkg/lib -llibrary'" to add
# libraries to the command line (do not use LDFLAGS on the command line, though
# it can be set in the environment)
#
# Use "CPPFLAGS+=-flags" to add CC flags (do not use CFLAGS on the command line,
# though it can be set in the environment)
#
# Use "CPPFLAGS+=-S -o -" with a ".o" target to see assembler
#
# Use "CSTD=c89 CPPFLAGS+='-pedantic'" to set a specific C standard.
#
# Use "OPTIM=-O0" to set the optimizer level
#
# Use "DBG=-g3" to set the debugger level for GCC
#
# Use "PEDANTIC=no" to turn off '-pedantic'
#
# See also "Makefile.compiler", e.g. for sanitizer options.

# This Makefile works with NetBSD Make, OSX bsdmake, Pkgsrc bmake (on OSX/Darwin
# one must set "SHLIB_MAJOR= SHLIB_MINOR= SHLIB_TEENY=" because Pkgsrc's
# bootstrap-mk-files (as of 20160411) are not yet fully ported to OSX/Darwin),
# and Simon Gerraty's Bmake & Mk-files from http://www.crufy.net/FreeWare/.
#
# Pkgsrc will install on a vast number of systems, including MS-Windows with
# Cygwin.  Simon's Bmake works on many Unix-like systems.

# You can define USE_UBSAN or USE_ASAN on the command line to enable the
# Undefined or Address sanitizers with recent enough Clang or GCC.  Note if your
# system supports these you may of course use its way of enabling them instead
# (e.g. on NetBSD set "MKSANITIZER=yes USE_SANITIZER=address").
#
# N.B.:  The address sanitizer is not available when the PaX ASLR (Address Space
# Layout Randomization) feature is enabled on a given system, but it can be
# disabled for a given binary with paxctl(8):  paxctl +a ${prog}

.if exists(/usr/share/mk/bsd.own.mk) || \
    exists(/usr/pkg/share/mk/bsd.own.mk) || \
    exists(/usr/pkg/share/mk/bsdmake/bsd.own.mk) || \
    exists(/usr/local/share/mk/bsd.own.mk) || \
    exists(/usr/local/share/mk/bsdmake/bsd.own.mk)
. include <bsd.own.mk>
.endif

# this will only have effect on some BSDs, not Darwin/OSX/macOS, nor FreeBSD,
# and it is added to LDFLAGS by the system mk-files (sys.mk in general, but also
# <bsd.prog.mk> sometimes.  Note that some of the sanitizers don't work when
# static-linked.
#
LDSTATIC ?= -static
# but this will work....
.if (defined(TARGET_OSNAME) && (${TARGET_OSNAME} == "Darwin")) || defined(.FreeBSD)
# for both FreeBSD and Mac OS X....
NO_SHARED = YES
.else
NOSHARED = YES
.endif

# experiments should be pedantic!
#
.if !defined(PEDANTIC) || ${PEDANTIC} != "no"
CWARNFLAGS += -pedantic
.endif

.include "Makefile.compiler"

# This is so we can use <bsd.prog.mk> without any manual page.  Note though that
# PROG is never defined -- this Makefile is intended to build a single program
# specified as the target on its command line.
#
MAN =	# empty

.if exists(/usr/share/mk/bsd.prog.mk) || \
    exists(/usr/pkg/share/mk/bsd.prog.mk) || \
    exists(/usr/pkg/share/mk/bsdmake/bsd.prog.mk) || \
    exists(/opt/pkg/share/mk/bsd.prog.mk) || \
    exists(/usr/local/share/mk/bsd.prog.mk) || \
    exists(/usr/local/share/mk/bsdmake/bsd.prog.mk)
. include <bsd.prog.mk>
.endif

# add patterns for our extra languages
#
PATTERNS ?=	Makefile CStandard.c g*.c t*.awk t*.[ch] t*.c++ t*.sh t*.clisp t*.mk t*.go go.mod go.sum

.if ((defined(TARGET_OSNAME) && (${TARGET_OSNAME} == "Darwin")) || \
	(defined(OS) && (${OS} == "Darwin")) || \
	(defined(.FreeBSD) && (empty(.FreeBSD) || (${.FreeBSD} == "false"))))
#
# xxx does Clang make .dSYM directories on FreeBSD too???
#
clean: 			macos-clean-dSYM
macos-clean-dSYM:	.PHONY
	rm -rf ${PROG}.dSYM
.endif

#DEST_RSH_HOSTS ?=	historically.local
DEST_OpenSSH_HOSTS ?=	very.local central.weird.com # freebsd.local mail.reptiles.org
#DEST_KLERVI_HOSTS ?=	faraday.klervi.com lithium.local

# seems to be a default target in <bsd.subdir.mk> on OSX (& FreeBSD?)
#
publish:	.PHONY
.for host in ${DEST_RSH_HOSTS}
	rsync -Ict --stats -e rsh ${PATTERNS} ${host}:src
.endfor
.for host in ${DEST_OpenSSH_HOSTS}
	rsync -Ict --stats -e ssh ${PATTERNS} ${host}:src
.endfor
.for host in ${DEST_KLERVI_HOSTS}
	rsync -Ict --stats -e "ssh -l gaw" ${PATTERNS} ${host}:src
.endfor

distribute:	.PHONY
distribute: publish

CHOST ?=	central.weird.com

# reverse tunnel:
#
#	cat >rsyncd.conf <<'__EOF__'		\
#	[src]					\
#		use chroot = false		\
#		read only = false		\
#		path = .			\
#	 __EOF__
#
rpull:	.PHONY
	rsync --no-detach --daemon --address 127.0.0.1 --port=22222 --config=./rsyncd.conf & \
	ssh -R 127.0.0.1:22222:127.0.0.1:22222 -l woods ${CHOST} 'cd src && rsync --stats --port=22222 ${PATTERNS} 127.0.0.1::src'; \
	kill $$!

pull:	.PHONY
.for pattern in ${PATTERNS}
	rsync -Ict --stats -e "ssh -l woods" ${CHOST}:src/${pattern} ./
.endfor
