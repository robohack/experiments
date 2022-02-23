#!/bin/sh

is_bash ()
{
	[ ${RANDOM:-0} -ne ${RANDOM:-0} -a -n "${BASH}" ]
}
is_bash && echo "Bash: ${BASH_VERSION}" || echo "not bash"

is_attksh ()
{
	is_bash && return 1

	# ksh93 and [p]bosh
	_sh_version=`eval 'echo "${.sh.version}"' 2>/dev/null`
	# [p]bosh only
	_sh_shell=`eval 'echo "${.sh.shell}"' 2>/dev/null`

	test -n "${_sh_version}" -a -z "${_sh_shell}"
	_rc=$?
	unset _sh_shell _sh_version

	return ${_rc}
}
is_attksh && echo "AT&T Ksh${_sh_version:+: }${_sh_version:- (probably ksh88 or ksh86)}" || echo "not real ksh"

is_zsh()
{
	test -n "${ZSH_VERSION}"
}
is_zsh && echo "Zsh: ${ZSH_VERSION}" || echo "not zsh"


defun_local="local () { :; }"

# every truly POSIX-compliant shell implements the built-in
# command 'local', but then there's ksh (even #93!)
#
# XXX the POSIX version of bosh (pbosh) fails this test and doesn't have typeset
#
if is_attksh; then		# if type local >/dev/null 2>&1; then
	#
	# AT&T Ksh93:
	#
	#	typeset vname[=value]
	#
	#	When invoked inside a function defined with the
	#	function name syntax, a new instance of the variable
	#	'vname' is created, and the variable's value and type
	#	are restored when the function completes.
	#
	# XXX Sadly this does not actually work for functions defined using the
	# "func() {body}" syntax, and that's the only portable way to define a
	# function!
	#
	# XXX so don't shadow globals in "func(){}" functions!!!
	#
	# Note that in AT&T Ksh (and others?) aliases are resolved when a
	# function definition is first read in, so set this early!
	#
	# I used to like AT&T Ksh -- now I'm glad it's only an inspiration...
	#
	alias local='typeset'
else
	# N.B.:  ancient "ash" had no 'type' builtin, though I implemented one
	# long ago.  It is in ~/.ashtype, with this to load it from ~/.profile
	# (see github.com/robohack/dotfiles)

	# the I/O re-direction doesn't actually get rid of the "type: not found" message
	# from the old Ash implementation...  Perhaps it parses the whole line first?
	#
	# XXX but maybe it would if we redirected stderr (via stdout?) within
	# the subshell too, as we must do for GNU Bash?
	#
	if ( type type > /dev/null 2>1 ) > /dev/null 2>&1 ; then
		:
	elif [ -r ${HOME}/.ashtype ]; then
		. ${HOME}/.ashtype
	fi

	if type local >/dev/null 2>&1; then
		:
	else
		# for Bourne (heirloom) sh, pbosh, etc.(?)
		#
		# This definition of "local" won't actually do anything useful,
		# but it also won't cause a syntax error either.
		#
		# xxx even though this is in an 'if' that souldn't be executed
		# by it, some versions of 'dash', aka "Debian Almquist shell"
		# (which started life as NetBSD sh) will barf and abort just
		# seeing this function definition for 'local' here!!!
		# (dash-0.5.5.1 is OK, dash-0.5.9.1 through 0.5.10.2 are STUPID)
		#
		# even worse it is almost impossible to programatically detect
		# dash programatically.  However luckily we can hide it in a
		# variable value....
		#
		eval ${defun_local}
	fi
fi

bar=none
ebar=enone
export ebar
lbar=nope

testit ()
{
	local lbar

	bar=some
	ebar=esome
	lbar=lsome
}

echo "before:"
echo bar=$bar
echo ebar=$ebar
echo lbar=$lbar

testit

echo "after:"
if [ $bar != some ]; then
	echo ERROR:  OOPS!!! bar=$bar
else
	echo bar=$bar
fi
if [ $ebar != esome ]; then
	echo ERROR:  OOPS!!! ebar=$ebar
else
	echo ebar=$ebar
fi
if [ $lbar = lsome ]; then
	echo ERROR:  OOPS!!! lbar=$lbar
else
	echo lbar=$lbar
fi

echo ""

tvar=foo

tfun()
{
	local tvar
	tvar=bar
	echo "Hi from tfun!  tvar=${tvar}"
}
tfun

echo tvar=${tvar}

# XXX this is really disturbing!!  It is not safe to put the opening paren on
# its own line as that would lead many non-AT&T-Ksh shells to just say
# "function:  not found" and then execute the function body immediately as if it
# were a normal compound command AND THEN CONTINUE RUNNING THE SCRIPT!!!
#
# Some though, like "bosh" and "dash", complain and die when seeing "local"
# oustide of a function so don't continue and run the script body.
#
# at least with the opening paren up on the definition line, those non-AT&T-Ksh
# (i.e. not fully compatible) shells like "sh" and "pbosh" will stop (after
# executing the function body) with a syntax error at the closing paren.
#
function tfun2	{
	local tvar
	tvar=none
	echo "Hi from tfun2!  tvar=${tvar}"
}				# xxx non-Ksh/bash barfs here!
tfun2

echo tvar=${tvar}

# Change the leading "sh" to whatever other shell you want to test...
#
# Local Variables:
# eval: (make-local-variable 'compile-command)
# compile-command: (concat "sh ./" (file-name-nondirectory (buffer-file-name)))
# End:
