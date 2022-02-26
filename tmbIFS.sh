#!/bin/sh

#
#	Test multibyte support in available shells
#
# Idea from Jörg Schilling:  https://sourceforge.net/p/schilytools/tickets/10/#6d41
#
# A reasonably Bourne compatible shell should run this script, but it need not
# be multibyte-capable itself.
#
# N.B.:  this file is encoded for LC_CTYPE=en_US.UTF-8
LC_CTYPE="en_US.UTF-8"
export LC_CTYPE

mktable ()
{
	sed				\
		-e '/^[ 	]*#/d'	\
		-e '/^[ 	]*$/d'	\
		${1+"$@"}
}

shells=${shells:-`mktable /etc/shells`}
mbshells=""

maxw=`echo $shells | awk -v RS=' ' 'BEGIN { m=0 } {l=length($0); if (l > m) { m=l } } END { print m+1 }'`
#echo maxw=$maxw

# note:  NetBSD's wc(1) has had multibyte support since 1.6, i.e. September 2002.

for tsh in $shells; do
	btsh=`basename $tsh`
	# rc is of course Plan 9 rc, and not at all POSIX compatible
	# osh is the Sixth Edition (v6) shell, and not remotely modern enough
	if [ -x $tsh -a $btsh != "rc" -a $btsh != "osh" ]; then
		printf "%*s: " $maxw $tsh
		LC_CTYPE="en_US.UTF-8" $tsh -c 'IFS=ö; A=Jörg; echo $A'
		len=`printf "%s\n" 'Jörg' | wc -m`
		li=`$tsh -c 'A=Jörg; printf "%s\n" $A | wc -m'`
		lo=`$tsh -c 'IFS=ö; A=Jörg; echo $A' | wc -m`
		printf "%*s: len=%d, li=%d, lo=%d\n" $maxw $tsh $len $li $lo
		if [ $len -ne $li ]; then
			printf "WARNING: '%s' does something weird without changing IFS!\n" $tsh
		elif [ $li -ne $lo ]; then
			printf "NOTICE: '%s' does not support multibyte characters!\n" $tsh
		else
			mbshells="${mbshells} ${tsh}"
		fi
	fi
done

printf "\nMultiByte-Shells: %s\n" "${mbshells}"


# Local Variables:
# eval: (make-local-variable 'compile-command)
# compile-command: (concat "sh ./" (file-name-nondirectory (buffer-file-name)))
# End:
