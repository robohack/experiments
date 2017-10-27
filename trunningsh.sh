#! /bin/sh
: || exec /bin/sh $0 $argv:q
# uncomment the following if you have trouble with older ksh's on USG systems
#(alias) >/dev/null 2>&1 && exec /bin/sh "$0" ${1+"$@"}
#
# This is a potential way to test for broken /bin/csh's (older XENIX?)
#true || exec /bin/sh $0 $argv:q
#
# another way to test if running with a Bourne-compatible sh or not
#export PATH || exec /bin/sh $0 $argv:q

# See the following for a decent explanation of #! interpreter script
# handling:
#
#	http://www.in-ulm.de/~mascheck/various/shebang/
#
# See the following for a good explanation of the how and why of ${1+"$@"}
#
#	http://www.in-ulm.de/~mascheck/various/bourne_args/

# xxx as seen below this does not work as one might expect/hope
PLAIN_ARGV=${@}

# carefully preserve original arguments individually with newline separation
# (in case they contain spaces)
ORIG_ARGV=""
for var in "${@}"; do
	ORIG_ARGV="${ORIG_ARGV}
${var}"
done

echo $0: \${1+\"\${@}\"}: ${1+"${@}"}
echo $0: \"\${@}\": "${@}"

# remove all arguments after any that is "n" (ash/sh set "n" to empty)
# xxx pdksh cannot do this and gives a 'bad substitution' error
# xxx ksh93 and bash only set the "n" to emptyness leaving rest
# xxx dash (0.5.7-3) does not emit the empty argument in place of "n"
echo $0: \${1+\"\${@%n}\"}: ${1+"${@%n}"}
echo $0: \"\${@%n}\": "${@%n}"

echo ""
echo "all-args:"

c=0
for i in "${@}"; do
	c=$(expr ${c} + 1)
	echo "arg #${c}: \"${i}\""
done

# xxx this may or may not be right...
if [ ${#} -ne ${c}  ]; then
	echo "WARNING:  your shell needs to use the \${1+\"\${0)\" trick!"
	exit 1
fi

echo ""
echo "arg-less-n:"

# remove all arguments after any that is "n"
c=0
for i in "${@%n}"; do
	c=$(expr ${c} + 1)
	echo "arg-less-n #${c}: \"${i}\""
done

echo "again:"

c2=0
for i in ${1+"${@%n}"}; do
	c2=$(expr ${c2} + 1)
	echo "arg-less-n #${c2}: \"${i}\""
done

# xxx this may or may not be right...
if [ ${c2} -ne ${c}  ]; then
	echo "WARNING:  your shell needs to use the \${1+\"\${0%n)\" trick!"
	exit 1
fi

echo ""
echo "orig-via-plain:"
c=0
for i in ${PLAIN_ARGV}; do
	c=$(expr ${c} + 1)
	echo "args-via-plain #${c}: \"${i}\""
done


echo ""
echo "orig-via-var:"

Sifs=${IFS}
# set IFS to a newline as per ORIG_ARGV setup
IFS="
"
c=0
for i in ${ORIG_ARGV}; do
	c=$(expr ${c} + 1)
	echo "args-via-var #${c}: \"${i}\""
done
IFS=${Sifs}

echo ""
echo "orig-via-var-and-set:"

# properly pass parameters via a variable
# (see above for setting it with newlines)

# xxx Zsh 4.3.17) messes this up badly

Sifs=${IFS}
# set IFS to a newline as per ORIG_ARGV setup
IFS="
"
set -- ${ORIG_ARGV}
IFS=${Sifs}

c=0
for i in "${@}"; do
	c=$(expr ${c} + 1)
	echo "args-via-var-@ #${c}: \"${i}\""
done

echo ""
echo "arg-less-t-optarg:"

# properly remove an option and optarg from argv

Sifs=${IFS}
# set IFS to a newline as per ORIG_ARGV setup
IFS="
"
set --
skip_optarg=false
# n.b.:  do not quote this expansion!
for var in ${ORIG_ARGV}; do
	if ${skip_optarg}; then
		skip_optarg=false
		echo "skipping optarg: '${var}'"
		continue
	fi
	if [ "${var}" = '-t' ]; then
		# let us say '-t' must have an optarg
		skip_optarg=true
		echo "skipping option: '${var}'"
		continue
	fi
	echo "setting param: '${var}'"
	# ${var} need not be quoted, but perhaps it is clearer
	set -- "${@}" "${var}"
done
IFS=${Sifs}
echo ""
c=0
for i in "${@}"; do
	c=$(expr ${c} + 1)
	echo "arg-less-'-t optarg' #${c}: \"${i}\""
done

# Local Variables:
# eval: (make-local-variable 'compile-command)
# compile-command: (concat "sh ./" (file-name-nondirectory (buffer-file-name)) " \"x y\" -t foo \"z z\" n -t \"foo bar\"")
# End:
