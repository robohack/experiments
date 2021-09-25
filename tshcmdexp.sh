#!/bin/ksh

# "The results of command substitution shall not be processed for
# further tilde expansion, parameter expansion, command substitution,
# or arithmetic expansion. If a command substitution occurs inside
# double-quotes, field splitting and pathname expansion shall not be
# performed on the results of the substitution."

NONE="foo  bar  "
echo "'${NONE}'"

BAR=$(echo "foo  bar  ")
echo "'${BAR}'"

BAR="$(echo "foo  bar  ")"
echo "'${BAR}'"

NONE=${BAR}$(echo "got  you ! ")
echo "'${NONE}'"

NONE="${BAR}"$(echo "got  you ! ")
echo "'${NONE}'"

NONE=${NONE}${NONE:+ }
echo "'${NONE}'"

# first discovered this problem with ksh trying the following:
#
#MY_IP=$(/sbin/ifconfig -l | tr ' ' '\012' | while read ifname; do case $ifname in en*) /sbin/ifconfig $ifname inet;; esac; done | awk '$1 == "inet" {print $2; exit 0}')
#
# it works fine without the $() command substitution wrapper.
#
# From the Single UNIX Specification Issue 6:
#
# "With the $(command) form, all characters following the open
# parenthesis to the matching closing parenthesis constitute the
# command.  Any valid shell script can be used for command, except a
# script consisting solely of redirections which produces unspecified
# results."

# KSH Version M 1993-12-28 s+		FAIL
# PD KSH v5.2.14 99/07/13.2		FAIL
# NetBSD-4.x /bin/sh			works!
# BASH 3.2.48(1)-release		FAIL
# ZSH 4.3.9				FAIL

echo foo | while read var; do case $var in fo*) echo it plainly works;; esac; done
BAR=`echo foo | while read var; do case $var in fo*) echo it works old style;; esac; done`
echo $BAR
BAR=$(echo foo | while read var; do case $var in fo*) echo it works the ksh/posix way;; esac; done)
echo $BAR
