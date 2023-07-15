# -*- makefile-bsdmake -*-
#
#	Makefile - of the BSD variety
#
# (on older Mac OS X, use "bsdmake"; on macOS install "bmake")
#
# Use "LDLIBS+=-llibrary" to add libraries to the command line
#
# Use "CPPFLAGS+=-flags" to add CC flags (do not use CFLAGS on the command line,
# though it can be set in the environment)
#
# Use "CPPFLAGS+=-S -o -" with a ".o" target to see assembler
#

# This Makefile works with NetBSD Make, OSX bsdmake, Pkgsrc bmake (on OSX/Darwin
# one must set "SHLIB_MAJOR= SHLIB_MINOR= SHLIB_TEENY=" because Pkgsrc's
# bootstrap-mk-files (as of 20160411) are not yet fully ported to OSX/Darwin),
# and Simon Gerraty's Bmake & Mk-files from http://www.crufy.net/FreeWare/.
#
# Pkgsrc will install on a vast number of systems, including MS-Windows with
# Cygwin.  Simon's Bmake works on many Unix-like systems.

.if exists(/usr/share/mk/bsd.own.mk) || \
    exists(/usr/pkg/share/mk/bsd.own.mk) || \
    exists(/usr/pkg/share/mk/bsdmake/bsd.own.mk) || \
    exists(/usr/local/share/mk/bsd.own.mk) || \
    exists(/usr/local/share/mk/bsdmake/bsd.own.mk)
. include <bsd.own.mk>
.endif

# This is so we can use <bsd.prog.mk> without any manual page.  Note though that
# PROG is never defined -- this Makefile is intended to build a single program
# specified as the target on its command line.
#
MAN =	# empty

#
# find all CC pre-defines with:
#
#	echo "" | cc -E -dM -x c - | sort
#
# to see what specific optimizations are enabled by -O*:
#
#	echo "" | gcc -O[0123] -S -fverbose-asm -o - -x c -
#
# to show search paths and "as" and linker commands:
#
#	echo "" | gcc -v -x c -
#
#
# N.B.:  these tricks do not work for lcc

.if !defined(__GNUC__)
__GNUC__ !=		echo __GNUC__ | $(CC) -E -c - | sed '/\#/d;s/__GNUC__//'
.endif
.if empty(__GNUC__)
. undef __GNUC__
.else
. if !defined(__GNUC_MINOR__)
__GNUC_MINOR__ !=	echo __GNUC_MINOR__ | $(CC) -E -c - | sed '/\#/d;s/__GNUC_MINOR__//'
. endif
. if !defined(__GNUC_PATCHLEVEL__)
__GNUC_PATCHLEVEL__ !=	echo __GNUC_PATCHLEVEL__ | $(CC) -E -c - | sed '/\#/d;s/__GNUC_PATCHLEVEL__//'
. endif
.endif

# Note: for now Clang also defines the __GNUC*__ macros above...

# Note: on all recent Mac OS X (and soon FreeBSD), "cc" is "clang"
# On Mac OS X 10.8 "gcc" is actually "llvm-gcc" too!

.if !defined(__clang__)
__clang__ !=		echo __clang__ | $(CC) -E -c - | sed '/\#/d;s/__clang__//'
.endif
.if empty(__clang__)
. undef __clang__
.else
. if !defined(__clang_major__)
__clang_major__ !=	echo __clang_major__ | $(CC) -E -c - | sed '/\#/d;s/__clang_major__//'
. endif
. if !defined(__clang_minor__)
__clang_minor__ !=	echo __clang_minor__ | $(CC) -E -c - | sed '/\#/d;s/__clang_minor__//'
. endif
. if !defined(__clang_patchlevel__)
__clang_patchlevel__ !=	echo __clang_patchlevel__ | $(CC) -E -c - | sed '/\#/d;s/__clang_patchlevel__//'
. endif
.endif

.if ((defined(_HOST_OSNAME) && (${_HOST_OSNAME} == "Darwin")) || \
	(defined(OS) && (${OS} == "Darwin")) || \
	(defined(.FreeBSD) && (${.FreeBSD} == "false")))
#
# NOTE: Mac OS X does not use GNU ld, but....
#
.else
. if !defined(__GNULD__)
__GNULD__ !=		$(LD) -v | awk '{print int($$4);}' | tr -d ' \012'
. endif
.endif

NOGCCERROR = 1
WARNS = 0

# -Winline
#
#	- seems pointless
#
# -Wnested-externs
#
#	- possibly useful, though -Wshadow covers worst case
#	- see tnested-externs.c
#
# -Wredundant-decls
#
#	- seems pointless


.if defined(__GNUC__) && ${__GNUC__} >= 1
CWARNFLAGS += -Wall
# this one is just plain annoying much of the time:
CWARNFLAGS += -Wno-missing-field-initializers
# these aren't always in "-Wall"
CWARNFLAGS += -Wimplicit
CWARNFLAGS += -Wreturn-type
CWARNFLAGS += -Wswitch
CWARNFLAGS += -Wcomment
CWARNFLAGS += -Wcast-qual
CWARNFLAGS += -Wpointer-arith
CWARNFLAGS += -Wshadow
# xxx some code may not be quite ready for full const-ification yet...
CWARNFLAGS += -Wwrite-strings
# this isn't mentioned in 1.42's manual but causes no error
CWARNFLAGS += -Wstrict-prototypes
#XXX# -Wsystem-headers does not go well with -Wundef
#XXX#CWARNFLAGS += -Wsystem-headers
.endif # ${__GNUC__} >= 1

# in addition to -Wall
#
.if defined(__GNUC__)
. if ${__GNUC__} <= 2
# "-Wid-clash-LEN" is sadly no longer supported in 3.2.2 or 3.3.3
# (but is still in 3.0.4)
CWARNFLAGS += -Wid-clash-30
. endif
.if ${__GNUC__} > 3 || \
	(${__GNUC__} == 3 && ${__GNUC_MINOR__} == 4 && ${__GNUC_PATCHLEVEL__} >=6) || \
	(${__GNUC__} == 3 && ${__GNUC_MINOR__} >= 5)
# "-W" was replaced by "-Wextra" since at least 3.4.6
CWARNFLAGS += -Wextra
. else
CWARNFLAGS += -W
. endif
.endif

.if defined(__GNUC__) && \
	${__GNUC__} >= 2
# functions are SUPPOSED to be able to return structs!!!
#CWARNFLAGS += -Waggregate-return
CWARNFLAGS += -Wcast-align
CWARNFLAGS += -Wchar-subscripts
# -Wconversion changes meanings in GCC-4.4 and newer, but we may still want it
CWARNFLAGS += -Wconversion
CWARNFLAGS += -Wmissing-declarations
CWARNFLAGS += -Wmissing-prototypes
CWARNFLAGS += -Wno-long-long
# the rest aren't in the manual, but work on at least 2.9x.x
CWARNFLAGS += -Wformat-extra-args
# -Wundef isn't in 2.7.2, but then again if you're still using 2.7.2 you may be
# suffering from far more code generation bugs and other compiler problems than
# you might know!
CWARNFLAGS += -Wundef
# we may intentionally have large arrays
#CWARNFLAGS += -Wlarger-than-65536
# N.B.:  -Wbad-function-cast can be extremely annoying as it complains about
# conversions by casts which even -Wconversion doesn't complain about.  In
# theory it does warn about cases where UB(?)/IDB can happen because the return
# value is out-of-bounds for the target type.
CWARNFLAGS += -Wbad-function-cast
.endif

.if defined(__GNUC__) && \
	${__GNUC__} >= 3
# Yuck:  this is broken in at least 3.2.2...
#CWARNFLAGS += -Wunreachable-code
CWARNFLAGS += -Wdeclaration-after-statement
# -Wformat is included in -Wall, but not "-Wformat-nonliteral -Wformat-security"
# but -Wformat-y2k is stupid
CWARNFLAGS += -Wformat=2 -Wno-format-y2k
# note: will only have effect if '-fstrict-aliasing' is enabled, but we never
# enable that option!
# xxx maybe only in >= 3.3?
#CWARNFLAGS += -Wstrict-aliasing=2
.endif

.if (defined(__GNUC__) && \
	${__GNUC__} >= 4 && \
	!defined(__clang__)) || \
	(defined(__clang__) && defined(__clang_major__) && ${__clang_major__} > 1)

. if(${__GNUC_MINOR__} >= 4)
# hmmm...  this can sometimes be annoying, but....
CWARNFLAGS += -Wtraditional-conversion
. endif
CWARNFLAGS += -Wunreachable-code
# XXX this only works if we don't turn off -fstrict-overflow _and_ we have -O2
# or greater (see below)
#CWARNFLAGS += -Wstrict-overflow=5
.endif

# maybe this is right? (it's always enabled in more recent GCC's)
.if !empty(unix:M*NetBSD*) && ${__GNUC__} < 3 && ${__GNUC__} > 2
CWARNFLAGS += -Wnetbsd-format-audit
.endif

# note: since Clang also defines the __GNUC__ macros we just need to add
# Clang-specific flags here and we get all the GCC warning flags for free...
.if defined(__clang__)
# get all the info!
CWARNFLAGS += -fmessage-length=0
# try this -- good for tests, bad for production!!!
. if ${__clang_major__} <= 5
CWARNFLAGS += -fcatch-undefined-behavior
.  if ${__clang_major__} == 4 || (${__clang_major__} == 3 && ${__clang_minor__} >= 3)
# i.e. clang 3.3 or clang 4.x (but not clang 5.x)
CWARNFLAGS += -fsanitize=undefined
.  endif
. else
.  if ${__clang_major__} >= 7
# N.B.:  It is not possible to combine more than one of the -fsanitize=address,
# -fsanitize=thread, and -fsanitize=memory checkers in the same program.
#xxx 'address' may not work (requires intrumentation of all program code)
#CWARNFLAGS += -fsanitize=address
##CWARNFLAGS += -fsanitize-address-use-after-scope
# sadly Apple LLVM version 8.1.0 (clang-802.0.42) says:
#   clang: error: unsupported option '-fsanitize=memory' for target 'x86_64-apple-darwin16.6.0'
# manual says:  MemorySanitizer is supported on Linux x86_64/MIPS64/AArch64
#CWARNFLAGS += -fsanitize=memory -fsanitize-memory-track-origins=2
#CWARNFLAGS += -fsanitize=thread
CWARNFLAGS += -fsanitize=undefined
CWARNFLAGS += -fno-omit-frame-pointer
# Apple LLVM version 8.1.0 (clang-802.0.42) says:
# ld: file not found: /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/../lib/clang/8.1.0/lib/darwin/libclang_rt.safestack_osx.a
# clang: error: linker command failed with exit code 1 (use -v to see invocation)
#CWARNFLAGS += -fsanitize=safe-stack
#xxx this may not be useful?
#CWARNFLAGS += -fsanitize=dataflow --fsanitize=cfi
.  else
# clang > 5.0 < 7.x(?)
# Apple LLVM version 6.0 (clang-600.0.57) (based on LLVM 3.5svn) says:
#  clang: error: unsupported argument 'undefined' to option 'fsanitize='
#CWARNFLAGS += -fsanitize=undefined
.  endif
. endif
# n.b.:  for single-file targets this is a bit of an over-kill, but....
# -fsanitize must also be provided to the linker (for hidden runtime libraries)
LDFLAGS += ${CWARNFLAGS:M-fsanitize=*}
.endif

.if defined(__GNUC__) || defined(__clang__)
# WARNING: C99 (and C11) allow compilers to perform optimizations based on the
# "strict" aliasing, overflow, and enums rules which _will_ change the behaviour
# of previously correct C90 and earlier code!
CFLAGS += -fno-strict-aliasing

. if (defined(__GNUC__) && \
	${__GNUC__} >= 4 && ${__GNUC_MINOR__} >= 2)
# WARNING:  Prevent the optimizer from assuming that the program
# follows the strict signed overflow semantics permitted for the
# language.
CFLAGS += -fno-strict-overflow
. endif
.endif

# XXX in GCC -fstrict-enums apparently only applies to C++
.if defined(__GNUC__) && defined(__clang__)
# WARNING:  Prevent the compiler from optimizing "using the assumption
# that a value of enumerated type can only be one of the values of the
# enumeration"
CFLAGS += -fno-strict-enums
.endif

.if (defined(__GNUC__) && !defined(__clang__) && \
	${__GNUC__} >= 4 && ${__GNUC_MINOR__} >= 2)
# WARNING:  Force the compiler to always read the whole underlying value!
CFLAGS += -fno-strict-volatile-bitfields
.endif

# N.B.:  -g3 is required if debugging is wanted while optimizing with -O2.
DBUG ?= -g
OPTIM ?= # empty
LDSTATIC ?= -static

.if (defined(_HOST_OSNAME) && (${_HOST_OSNAME} == "Darwin")) || defined(.FreeBSD)
# for both FreeBSD and Mac OS X....
NO_SHARED = YES
CFLAGS += ${DBUG}
CFLAGS += ${OPTIM}
CFLAGS += ${CPPFLAGS}
.else
NOSHARED = YES
.endif

.if defined(__GNUC__) && ${__GNUC__} >= 1
. if defined(__GNULD__) && ${__GNULD__} >= 1
LDFLAGS += -Wl,--warn-common
#LDFLAGS += -Wl,--fatal-warnings
LDFLAGS += -Wl,--unresolved-symbols=report-all
#LDFLAGS += -Wl,--error-unresolved-symbols
. else
.  if !empty(${DBUG:M-g})
# hmmm....  GNU ld(1) doesn't need -g, but some linkers do
#LDFLAGS += -Wl,-g
.  endif
. endif
.endif

.if exists(/usr/share/mk/bsd.prog.mk) || \
    exists(/usr/pkg/share/mk/bsd.prog.mk) || \
    exists(/usr/pkg/share/mk/bsdmake/bsd.prog.mk) || \
    exists(/opt/pkg/share/mk/bsd.prog.mk) || \
    exists(/usr/local/share/mk/bsd.prog.mk) || \
    exists(/usr/local/share/mk/bsdmake/bsd.prog.mk)
. include <bsd.prog.mk>
.endif

.if (defined(__GNUC__) && ${__GNUC__} >= 1) || \
	(defined(__clang__) && ${__clang__} >= 1)
. if empty(CFLAGS:M*-pipe*)
CFLAGS +=	-pipe
. endif
.endif

# Dunno for sure if this is right, but Apple's PowerPC GCC-4.0.1 does not have
# stack-protector, however 4.1.x on many systems does....
#
# (gcc 4.4.3 from pkgsrc does not, sadly, as it is forced off in the
# build, but oddly it doesn't complain about the option -- it just
# fails to link programs because the option causes it to ask the
# linker for -lssp)
#
.if defined(__GNUC__) && \
	(${__GNUC__} > 4 || (${__GNUC__} == 4 && ${__GNUC_MINOR__} > 0)) && \
	empty(CFLAGS:M*-fstack-protector-all*)
CFLAGS +=	-fstack-protector-all
.endif

# XXX should probably consider adding the following, at least for GCC to avoid
# the worst cases of allowing the optimizer to abuse "undefined behaviour" in
# counter-productive ways (bugs should be bugs!):
#
#	-fwrapv -fno-strict-overflow -fno-delete-null-pointer-checks -fno-aggressive-loop-optimizations
#
# XXX the problem is we don't have a firm list of which optimizations are "total
# license" optimizations that abuse "undefined behaviour"
#
# XXX see also:  https://stackoverflow.com/questions/33278757/disable-all-optimization-options-in-gcc
#
# XXX worse yet GCC (and perhaps Clang) only warn about certain issues IFF they
# are abusing them.  For example GCC's -Wstrict-overlfow only works if
# -fstrict-overflow is enabled _and_ you have enabled a sufficient level of
# optimization to trigger it.  WTFH!?!?!?  The manual at least states this:
#
#	Note that it does not warn about all cases where the code might
#	overflow: it only warns about cases where the compiler implements some
#	optimization.  Thus this warning depends on the optimization level.


.if !empty(CFLAGS:M*-fstack-protector*) && \
	empty(CFLAGS:M*-Wstack-protector*) && \
	defined(CWARNFLAGS) && \
	empty(CWARNFLAGS:M*-Wstack-protector*)
CWARNFLAGS +=	-Wstack-protector
.endif

# this is a hack for my own NetBSD releases where older versions don't include
# CPPFLAGS in the LINK macro, so the ".c:" rule won't get CPPFLAGS (note: it's
# tempting to include "&& empty(LINK.c:M*CPPFLAGS*)" in the expression, but that
# won't work because empty() expands the variable entirely before applying
# modifiers.)
.if !empty(unix:M*NetBSD*) && !defined(COMPILE_LINK.c)
LDFLAGS += ${CPPFLAGS}
.endif

# Since we do not define PROG, this allows builds with a NetBSD cross-compiler
# toolchain (i.e. with "${TOOLDIR}/bin/nbmake-${MACHINE}")
.if !empty(_CCLINKFLAGS)
LDFLAGS += ${_CCLINKFLAGS}
.endif

PATTERNS ?=	Makefile CStandard.c g*.c t*.awk t*.[ch] t*.c++ t*.sh t*.clisp t*.mk t*.go go.mod go.sum

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
