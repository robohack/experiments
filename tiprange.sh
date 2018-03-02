#!/bin/ksh

ip="192.168.240.240"

num=40

# to detect array support
#
# See https://unix.stackexchange.com/a/238129
# (but the test there is not pdksh friendly)
#
{
	unset arr
	# xxx the 'set -A' causes many POSIX-like shells to die
	set -A arr a a || eval "arr=(a b)"
	eval '[ -n "${arr[1]}" ]'
	eval 'res="${arr[1]}"'
	if [ "${res}" = "b" ]; then
		# bash is stupid
		bash_arrays=true
	else
		bash_arrays=false
	fi
} > /dev/null 2>&1
if [ $? -eq 0 ]; then
	array_supported=true
else
	array_supported=false
fi

echo array_supported=${array_supported}
echo bash_arrays=${bash_arrays}

if $bash_arrays; then
# XXX this is parsed by pdksh, and it causes a syntax error that aborts parsing
#	iparr=($(echo ${ip} | sed 's/\./ /g'))
	i=0
	declare -a iparr
	for e in $(echo ${ip} | sed 's/\./ /g')
	do
		iparr[$i]=$e
		let $((i++))
	done
else
	set -A iparr $(echo ${ip} | sed 's/\./ /g')
fi

nip=$((iparr[0] << 24))
nip=$((nip | iparr[1] << 16))
nip=$((nip | iparr[2] << 8))
nip=$((nip | iparr[3]))

# XXX mksh seems to use signed 32-bit integers, everyone else something better
echo "numeric IP:  ${nip}"

# xxx for POSIX shell and for this special case we could use the positional
# parameters to store array values....
#
#set -- $(echo ${ip} | sed 's/\./ /g')
#
#nip=$(($1 << 24))
#nip=$((nip | $2 << 16))
#nip=$((nip | $3 << 8))
#nip=$((nip | $4))
#
# N.B.:  we could even save $@ and restor it:
#
#save () {
#	for i do
#		printf %s\\n "$i" | sed "s/'/'\\\\''/g;1s/^/'/;\$s/\$/' \\\\/"
#	done
#	echo " "
#}
#origargv=$(save "$@")
#
# Do things:
#set -- foo bar baz boo
#
# restore:
#eval "set -- $origargv"
#
# See also https://github.com/makefu/array


lnip=$((nip + num))

i=1

# see tlocal.sh
is_attksh() {
	# ksh93
	_sh_version=`eval 'echo "${.sh.version}"' 2>/dev/null`
	# pdksh only
	_opt_login=`set -o 2>/dev/null | grep login`

	test -n "${_sh_version}" -o \( -z "${_opt_login}" -a -n "${_}" -a -n "${ERRNO}" -a -n "${FCEDIT}" -a -n "${PS3}" \)
}

while [ $nip -lt $lnip ]; do
	let $((nip++))
	# XXX some builds of ksh93 (lack of long double?) fail badly since
	# arithmetic expressions are floating point quantities...
	is_attksh && echo "numeric IP:  ${nip}"
	# XXX ... but although bitwise operations are "defined" in some sense,
	# they don't always do what one might expect
	iparr[0]=$((nip >> 24))
	iparr[1]=$(((nip >> 16) & 255))
	iparr[2]=$(((nip >> 8) & 255))
	iparr[3]=$((nip & 255))
	printf "#%04d: %d.%d.%d.%d\n" $i ${iparr[0]} ${iparr[1]} ${iparr[2]} ${iparr[3]}
	let $((i++))
done
