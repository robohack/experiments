#! /bin/ksh

# first discovered this problem with ksh trying the following:
#
#MY_IP=$(/sbin/ifconfig -l | tr ' ' '\012' | while read ifname; do case $ifname in en*) /sbin/ifconfig $ifname inet;; esac; done | awk '$1 == "inet" {print $2; exit 0}')
#
# it works fine without the $() command substitution wrapper.

# KSH Version M 1993-12-28 s+		FAIL
# PD KSH v5.2.14 99/07/13.2		FAIL
# NetBSD-4.x /bin/sh			WORKS!
# BASH 3.2.48(1)-release		FAIL


echo foo | while read var; do case $var in fo*) echo it plainly works;; esac; done
BAR=`echo foo | while read var; do case $var in fo*) echo it works old style;; esac; done`
echo $BAR
BAR=$(echo foo | while read var; do case $var in fo*) echo it works the ksh/posix way;; esac; done)
echo $BAR
