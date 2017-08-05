#!/bin/sh

# the problem:  first command in a pipeline does not trigger exit
#
set -e
{ echo "Hello, '-e' is set and there follows a command which exits non-zero..."; false; } | cat
echo "but the pipeline exit status is:  $?"
( echo "Hello, '-e' is set and there follows a command which exits non-zero..."; exit 13 ) | cat
echo "and the explicit subshell pipeline exit status is:  $?"
set +e

# XXX For AT&T Ksh and Zsh the original solution requires an extra subshell
# around the pipeline else it behaves incorrectly.  The manual hints that set -o
# pipefail might help, but it does not seem to.  It does however prevent a new
# command from starting before the old one has finished (comment it out, or run
# it with ksh88, and watch the output from each test occasionally get mixed up).
#
# So, how to even tell if it is a real AT&T KSH?  .sh.version?  but it is a
# syntax error in most shells and not present in ksh88 or earlier!
# (it was first added in the 12/28/93 release of ksh, the first ksh93)
#
# XXX For some shells it is sufficient if the expansion of .sh.version is
# wrapped in a sub-shell as this will ditch any error message in anything but
# the real AT&T KSH.  However for NetBSD sh this is insufficient as it simply
# stops reading the script when it sees such a thing.  Also some versions of
# bash still spew the error.  The eval trick from here (in a comment to my
# comment) works:
#
#	https://unix.stackexchange.com/a/199936/29168
#
# xxx the first two cause NetBSD sh to bail immediately!
#_sh_version=$(echo "${.sh.version}"; true) 2>/dev/null
#_sh_version=$( (echo "${.sh.version}") 2>/dev/null ) 2>/dev/null
_sh_version=`eval 'echo "${.sh.version}"' 2>/dev/null`
# xxx maybe we could just test if "set -o pipefail" works and not care if it is ksh93?
if [ -n "${_sh_version}" ]; then
	echo "Setting 'pipefail' option for ksh93..."
	# n.b.:  as mentioned above this is not necessary for the failure
	# status, but rather just to get the shell to wait for all commands in
	# the pipeline to finish.
	set -o pipefail
fi
#
is_attksh()
{
	# N.B.:  we cannot use 'local' for these variables
	# (xxx well, we could if we tested for 'local' and 'typeset')

	# ksh93
	_sh_version=`eval 'echo "${.sh.version}"' 2>/dev/null`
	# pdksh only
	_opt_login=`set -o 2>/dev/null | grep login`

	test -n "${_sh_version}" -o \( -z "${_opt_login}" -a -n "${_}" -a -n "${ERRNO}" -a -n "${FCEDIT}" -a -n "${PS3}" \)
}
is_attksh && echo "AT&T Ksh${_sh_version:+: }${_sh_version:- (probably ksh88 or ksh86)}" || echo "not real ksh"

is_zsh()
{
	test -n "${ZSH_VERSION}"
}
is_zsh && echo "Zsh: ${ZSH_VERSION}" || echo "not zsh"

someprog()
{
	echo "line1"
	echo "line2"
	echo "line3"

	return 42
}

someprog3()
{
	echo "line1"
	# xxx only bash gives a (correct) line number for the next error
	# xxx pdksh gives the line number as the one where the invocation is
	# (others just mention the descriptor number which confusingly looks
	# like a line number)
	echo "line2" >&3
	# xxx ksh88 won't continue past the error above and returns 16
	echo "line3"

	return 99
}

filter()
{
	while read line; do
		echo "filtered $line"
	done

	return 13			# ignored
}

# return (as $?) the status of the first command in the pipeline
#
# idea from:  https://unix.stackexchange.com/a/70675/29168
#
# XXX maybe this could/should have a '-e' option to get the exit behaviour?
#
pipefail_run()
{
	if [ ${#} -lt 2 -o ${#} -gt 4 ]; then
		echo "Usage: pipefail_run producer [producer_arg] consumer [consumer_arg]" >&2
		exit 2
	fi
	_producer=${1}
	if [ ${#} -lt 3 ]; then
		_consumer=${2}
	else
		_producer_param=${2}
		_consumer=${3}
	fi
	if [ ${#} -gt 3 ]; then
		_consumer_param=${4}
	fi

	# The "producer" command's standard output is passed via FD#4 (which is
	# necessary because we have to send FD#3 to FD#1 of the exit status
	# getter, and the "produder" command's exit status is passed by echoing
	# it down FD#3 to an outer, final pipe reader script that converts it to
	# the exit status for the whole pipeline.  (If there had been a standard
	# syntax for getting a pipe to attach some other descriptor to FD#1 for
	# the "consumer", then use of FD#4 would be unnecessary.)

	# XXX for reasons I do not yet quite understand the use of an outer
	# subshell for AT&T ksh and Zsh is not necessary when this construct is
	# used inside a function.  Perhaps it is because functions are executed
	# inside subshells?

	{ { { { ${_producer} ${_producer_param:+${_producer_param}} 3>&- 4>&-
					_rc=${?}
					echo "pipefail_run(): producer exit status: ${_rc}" >&2
					echo ${_rc} >&3
				} | ${_consumer} ${_consumer_param:+${_consumer_param}} >&4
			} 3>&1
		} | {
			read _rc
			echo "pipefail_run(): using exit status: ${_rc}" >&2
			return ${_rc}
		}
		# N.B.:  There must not be any commands here!
		# (though perhaps they could be if ${?} were preserved)
	} 4>&1
	_rc=${?}
	# xxx POSIX compliant shells (and heirloom Bourne sh) show this with
	# 'set -e' set, due to ancient weirdness regarding how 'set -e'
	# shouldn't be "passed into" functions
	echo "pipefail_run(): returning: ${_rc}" >&2

	return ${_rc}
}

pipefun()
{
	set -e
	pipefail_run ${1:+"${@}"}
	_rc=${?}
	set +e
	echo "NOTICE:  you should not see this exit status: ${_rc}"

	return ${_rc}
}

echo ""
echo "and now a better (but unnecessarily expensive) way..."

( ( ( ( someprog; echo $? >&3; ) | filter >&4 ) 3>&1 ) | ( read xs; exit $xs ) ) 4>&1
res=$?
if [ ${res} -ne 42 ]; then
	echo "NOTICE: ${res} is not the correct exit status!"
else
	echo "the 'pipeline' exit status: ${res}"
fi

if is_attksh || is_zsh; then
	echo ""
	echo "Again for AT&T Ksh or zsh with an outer subshell..."

	( { { { someprog; echo $? >&3; } | filter >&4; } 3>&1; } | { read xs; exit $xs; } ) 4>&1
else
	echo ""
	echo "Again for normal shell using compound lists..."

	{ { { { someprog; echo $? >&3; } | filter >&4; } 3>&1; } | { read xs; exit $xs; }; } 4>&1
fi
res=$?
if [ ${res} -ne 42 ]; then
	echo "NOTICE: ${res} is not the correct exit status!"
else
	echo "the 'pipeline' exit status: ${res}"
fi

echo ""
echo "protecting from bad FDs..."

# If you worry that your someprog might write to file descriptor 3 or 4 then it
# is best to close the file descriptors before calling someprog.
#
if is_attksh || is_zsh; then
	( { { { someprog3 3>&- 4>&-; echo $? >&3; } | filter >&4; } 3>&1; } | { read xs; exit $xs; } ) 4>&1
else
	{ { { { someprog3 3>&- 4>&-; echo $? >&3; } | filter >&4; } 3>&1; } | { read xs; exit $xs; }; } 4>&1
fi
res=$?
if [ ${res} -ne 99 ]; then
	echo "NOTICE: ${res} is not the correct exit status!"
else
	echo "the 'pipeline' exit status: ${res}"
fi

echo ""
echo "and now using the helper function..."

# n.b.:  set -e will work here, but this script needs to show some more tests...
#set -e
pipefail_run someprog filter
res=$?
if [ ${res} -ne 42 ]; then
	echo "NOTICE: ${res} is not the correct exit status!"
else
	echo "the 'pipefail_run' exit status: ${res}"
fi


echo ""
echo "and now again, but with 'set -e'..."

pipefun someprog filter

echo "NOTICE:  You should never see this!"

set -e
if is_attksh || is_zsh; then
	( { { { someprog 3>&- 4>&-; echo $? >&3; } | filter >&4; } 3>&1; } | { read xs; exit $xs; } ) 4>&1
else
	{ { { { someprog 3>&- 4>&-; echo $? >&3; } | filter >&4; } 3>&1; } | { read xs; exit $xs; }; } 4>&1
fi
res=$?
echo "NOTICE:  you should not see this exit status: ${res}"

exit ${res}
