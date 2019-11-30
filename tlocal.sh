#!/bin/sh

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

# AT&T Ksh is (still, as of ksh93u+) the only shell I know of which has
# functions, and which has a 'typset' command, but which does not have a 'local'
# command to declare function-scope variables, nor a "built-in" alias for it
# either.  Sigh.
#
if is_attksh; then
	# not that this has the desired effect with AT&T ksh, since we don't
	# currently use the "function foo {}" syntax for defining functions, and
	# to do that we would have to somehow elide/ignore 'function' for other
	# shells which don't offer this syntax.
	#
	# XXX so don't shadow globals in functions!!!

	# xxx strictly speaking this probably should be a function that would
	# treat any use of options as a usage error, BUT even though this is in
	# an 'if' that wouldn't be executed by it, some versions of 'dash' will
	# barf on having a function definition trying to re-define the 'local'
	# builtin here!!!  (dash-0.5.5.1 is OK, dash-0.5.9.1 is stupid)

	alias local=typeset
else
	# N.B.:  ancient "ash" had no 'type' builtin, though I implemented one
	# long ago.  It is in ~/.ashtype, with this to load it from ~/.profile

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

	# for Bourne (heirloom) sh.
	if expr "`type local 2>/dev/null`" : 'local not found$' > /dev/null 2>&1 ; then
		# XXX see above about dash-0.5.9.1 stupidity
		local()
		{
			:
		}
	fi
fi

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
# its own line as that would lead any non-AT&T-Ksh shell to just say "function:
# not found" and then execute the function body immediately as if it were a
# normal compound command AND THEN CONTINUE RUNNING THE SCRIPT!!!
#
# at least with the opening paren up on the definition line, any non-AT&T-Ksh
# (i.e. not fully compatible) shell will stop (after executing the function body
# as individual commands) with a syntax error at the closing paren.
#
function tfun2 {
	local tvar
	tvar=none
	echo "Hi from tfun2!  tvar=${tvar}"
}
tfun2

echo tvar=${tvar}
