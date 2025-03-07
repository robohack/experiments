#!/bin/sh

TOP=/top

# Given a variable name in $1, modify the variable in place as follows:
# Convert possibly-relative path to absolute path by prepending
# ${TOP} if necessary.  Also delete trailing "/", if any.
#
resolvepath()
{
	local var="$1"
	local val
	eval val=\"\${${var}}\"
	# trim whitespace from suffix
	val=${val%%*[[:blank:]]}
	# trim whitespace from prefix
	val=${val##*[[:blank:]]}
	case "${val}" in
	/)
		;;
	/*)
		val="${val%/}"
		;;
	*)
		val="${TOP}/${val}"
		;;
	esac
	eval ${var}=\"\${val}\"
}

for i in " /usr/src" "	/usr/xsrc" \
	 "   /usr/src  " "		/usr/xsrc		" \
	 "   /usr/src" "		/usr/xsrc" \
	 " 	      /usr/src  	 	" "		/usr/xsrc  	 	" \
	 " 	 /usr/src" "	 	/usr/xsrc" \
	 " 	 ./usr/src" "	 	./usr/xsrc" \
	 " 	 src" "	 	xsrc" \
	 ; do
	 pname="$i"
	 resolvepath pname
	 printf "pname='%s'\n" "$pname"
done
